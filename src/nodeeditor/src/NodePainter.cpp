#include "NodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "StyleCollection.hpp"
#include "PortType.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"
#include "Node.hpp"
#include "FlowScene.hpp"

using QtNodes::NodePainter;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeState;
using QtNodes::NodeDataModel;
using QtNodes::FlowScene;

void
NodePainter::
paint(QPainter* painter,
      Node & node,
      FlowScene const& scene)
{
  NodeGeometry const& geom = node.nodeGeometry();

  NodeState const& state = node.nodeState();

  bool isSelected = node.nodeGraphicsObject().isSelected(); // this is kind of part of the state

  geom.recalculateSize(painter->font());

  //--------------------------------------------
  NodeDataModel const * model = node.nodeDataModel();

  drawNodeRect(painter, geom, model, isSelected);

  drawConnectionPoints(painter, geom, state, model, scene);

  drawFilledConnectionPoints(painter, geom, state, model);

  drawModelName(painter, geom, state, model);

  drawEntryLabels(painter, geom, state, model);

  drawResizeRect(painter, geom, model);

  drawValidationRect(painter, geom, model, isSelected);

  /// call custom painter
  if (auto painterDelegate = model->painterDelegate())
  {
    painterDelegate->paint(painter, geom, model);
  }
}


void
NodePainter::
drawNodeRect(QPainter* painter,
             NodeGeometry const& geom,
             NodeDataModel const* model,
             bool isSelected)
{
  NodeStyle const& nodeStyle = model->nodeStyle();

  auto color = isSelected
               ? nodeStyle.SelectedBoundaryColor
               : nodeStyle.NormalBoundaryColor;

  if (geom.hovered())
  {
    QPen p(color, nodeStyle.HoveredPenWidth);
    painter->setPen(p);
  }
  else
  {
    QPen p(color, nodeStyle.PenWidth);
    painter->setPen(p);
  }

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(2.0, geom.height()));

  gradient.setColorAt(0.0, nodeStyle.GradientColor0);
  gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  gradient.setColorAt(1.0, nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  float diam = nodeStyle.UseLegacyStyle ? nodeStyle.ConnectionPointDiameter : 0;

  QRectF boundary( -diam, -diam, 2.0 * diam + geom.width(), 2.0 * diam + geom.height());

  double const radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const& geom,
                     NodeState const& state,
                     NodeDataModel const * model,
                     FlowScene const & scene)
{
  NodeStyle const& nodeStyle      = model->nodeStyle();
  auto const     &connectionStyle = StyleCollection::connectionStyle();

  float diameter = nodeStyle.ConnectionPointDiameter;
  auto  reducedDiameter = diameter * 0.6;

  for(PortType portType: {PortType::Out, PortType::In})
  {
    size_t n = state.getEntries(portType).size();

    for (unsigned int i = 0; i < n; ++i)
    {
      QPointF p = geom.portScenePosition(i, portType);

      auto const & dataType = model->dataType(portType, i);

      bool canConnect = (state.getEntries(portType)[i].empty() ||
                         (portType == PortType::Out &&
                          model->portOutConnectionPolicy(i) == NodeDataModel::ConnectionPolicy::Many) );

      double r = 1.0;
      if (state.isReacting() &&
          canConnect &&
          portType == state.reactingPortType())
      {

        auto   diff = geom.draggingPos() - p;
        double dist = std::sqrt(QPointF::dotProduct(diff, diff));
        bool   typeConvertable = false;

        {
          if (portType == PortType::In)
          {
            typeConvertable = scene.registry().getTypeConverter(state.reactingDataType(), dataType) != nullptr;
          }
          else
          {
            typeConvertable = scene.registry().getTypeConverter(dataType, state.reactingDataType()) != nullptr;
          }
        }

        if (state.reactingDataType().id == dataType.id || typeConvertable)
        {
          double const thres = 40.0;
          r = (dist < thres) ?
                (2.0 - dist / thres ) :
                1.0;
        }
        else
        {
          double const thres = 80.0;
          r = (dist < thres) ?
                (dist / thres) :
                1.0;
        }
      }

      if (connectionStyle.useDataDefinedColors())
      {
        painter->setBrush(connectionStyle.normalColor(dataType.id));
      }
      else
      {
        painter->setBrush(nodeStyle.ConnectionPointColor);
      }

      painter->drawEllipse(p,
                           reducedDiameter * r,
                           reducedDiameter * r);
    }
  };
}


void
NodePainter::
drawFilledConnectionPoints(QPainter * painter,
                           NodeGeometry const & geom,
                           NodeState const & state,
                           NodeDataModel const * model)
{
  NodeStyle const& nodeStyle       = model->nodeStyle();
  auto const     & connectionStyle = StyleCollection::connectionStyle();

  auto diameter = nodeStyle.ConnectionPointDiameter;

  for(PortType portType: {PortType::Out, PortType::In})
  {
    size_t n = state.getEntries(portType).size();

    for (size_t i = 0; i < n; ++i)
    {
      PortIndex portIndex = static_cast<PortIndex>(i);
      QPointF p = geom.portScenePosition(portIndex, portType);

      if (!state.getEntries(portType)[i].empty())
      {
        auto const & dataType = model->dataType(portType, portIndex);

        if (connectionStyle.useDataDefinedColors())
        {
          QColor const c = connectionStyle.normalColor(dataType.id);
          painter->setPen(c);
          painter->setBrush(c);
        }
        else
        {
          painter->setPen(nodeStyle.FilledConnectionPointColor);
          painter->setBrush(nodeStyle.FilledConnectionPointColor);
        }

        painter->drawEllipse(p,
                             diameter * 0.4,
                             diameter * 0.4);
      }
    }
  }
}


void
NodePainter::
drawModelName(QPainter * painter,
              NodeGeometry const & geom,
              NodeState const & state,
              NodeDataModel const * model)
{
  NodeStyle const& nodeStyle = model->nodeStyle();

  Q_UNUSED(state);

  if (!model->captionVisible())
    return;

  QString const &name = model->caption();

  QFont f = painter->font();

  f.setBold(true);

  QFontMetrics metrics(f);

  auto rect = metrics.boundingRect(name);

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);

  if (nodeStyle.UseLegacyStyle) {
	  QPointF position((geom.width() - rect.width()) / 2.0,
		  (geom.spacing() + geom.entryHeight()) / 3.0);

	  painter->drawText(position, name);
  } else {
	  QRect area = QRect(geom.width() + nodeStyle.NameLeftMargin, 0, rect.width(), geom.height());
	  painter->drawText(area, Qt::AlignLeft | Qt::AlignVCenter, name);
  }

  f.setBold(false);
  painter->setFont(f);
}


void
NodePainter::
drawEntryLabels(QPainter * painter,
                NodeGeometry const & geom,
                NodeState const & state,
                NodeDataModel const * model)
{
  QFontMetrics const & metrics =
    painter->fontMetrics();

  for(PortType portType: {PortType::Out, PortType::In})
  {
    auto const &nodeStyle = model->nodeStyle();

    auto& entries = state.getEntries(portType);

    size_t n = entries.size();

    for (size_t i = 0; i < n; ++i)
    {
      PortIndex portIndex = static_cast<PortIndex>(i);
      QPointF p = geom.portScenePosition(portIndex, portType);

      if (entries[i].empty())
        painter->setPen(nodeStyle.FontColorFaded);
      else
        painter->setPen(nodeStyle.FontColor);

      QString s;

      if (model->portCaptionVisible(portType, portIndex))
      {
        s = model->portCaption(portType, portIndex);
      }
      else
      {
        s = model->dataType(portType, portIndex).name;
      }

      auto rect = metrics.boundingRect(s);

      p.setY(p.y() + rect.height() / 4.0);

      switch (portType)
      {
      case PortType::In:
        p.setX(5.0);
        break;

      case PortType::Out:
        p.setX(geom.width() - 5.0 - rect.width());
        break;

      default:
        break;
      }

      painter->drawText(p, s);
    }
  }
}


void
NodePainter::
drawResizeRect(QPainter * painter,
               NodeGeometry const & geom,
               NodeDataModel const * model)
{
  if (model->resizable())
  {
    painter->setBrush(Qt::gray);

    painter->drawEllipse(geom.resizeRect());
  }
}


void
NodePainter::
drawValidationRect(QPainter * painter,
                   NodeGeometry const & geom,
                   NodeDataModel const * model,
                   bool isSelected)
{
  auto modelValidationState = model->validationState();

  if (modelValidationState != NodeValidationState::Valid)
  {
    NodeStyle const& nodeStyle = model->nodeStyle();

    auto color = isSelected
                 ? nodeStyle.SelectedBoundaryColor
                 : nodeStyle.NormalBoundaryColor;

	if (nodeStyle.UseLegacyStyle)
	{
		QPen p(color, geom.hovered() ? nodeStyle.HoveredPenWidth : nodeStyle.PenWidth);
		painter->setPen(p);
	}
    else
    {
		painter->setPen(Qt::NoPen);
    }

    //Drawing the validation message background
    if (modelValidationState == NodeValidationState::Error)
    {
      painter->setBrush(nodeStyle.ErrorColor);
    }
    else
    {
      painter->setBrush(nodeStyle.WarningColor);
    }

    double const radius = 3.0;

    float diam = nodeStyle.ConnectionPointDiameter;

	if (nodeStyle.UseLegacyStyle) {
		QRectF boundary(-diam,
			-diam + geom.height() - geom.validationHeight(),
			2.0 * diam + geom.width(),
			2.0 * diam + geom.validationHeight());

		painter->drawRoundedRect(boundary, radius, radius);
	} else {
		painter->drawRoundedRect(geom.validationRect(), radius, radius);
	}

    //Drawing the validation message itself
    QString const &errorMsg = model->validationMessage();

    QFont f = painter->font();

    QFontMetrics metrics(f);

    auto rect = metrics.boundingRect(errorMsg);

	if (nodeStyle.UseLegacyStyle) {
		QPointF position ((geom.width() - rect.width()) / 2.0,
			geom.height() - (geom.validationHeight() - diam) / 2.0);

		painter->setFont(f);
		painter->setPen(nodeStyle.FontColor);
		painter->setBrush(Qt::gray);
		painter->drawText(position, errorMsg);
	} else {
		if (geom.validationHovered()) {
			QPoint c = geom.validationRect().center();
			QRect area = QRect(c.x(), c.y(), rect.width() + 10, geom.height());
			painter->drawRect(area);

			QRect textArea = QRect(area.left() + 5, area.top() + 5, area.width() - 10, area.height() - 10);
			painter->setFont(f);
			painter->setPen(nodeStyle.FontColor);
			painter->setBrush(Qt::gray);
			painter->drawText(textArea, Qt::AlignLeft | Qt::AlignVCenter, errorMsg);
		}
	}
  }
}
