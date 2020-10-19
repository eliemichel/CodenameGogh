#include "NodeGeometry.hpp"

#include <iostream>
#include <cmath>

#include "PortType.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"
#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeGeometry;
using QtNodes::NodeDataModel;
using QtNodes::PortIndex;
using QtNodes::PortType;
using QtNodes::Node;

NodeGeometry::
NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel)
  : _width(100)
  , _height(150)
  , _inputPortWidth(70)
  , _outputPortWidth(70)
  , _entryHeight(20)
  , _spacing(20)
  , _hovered(false)
  , _validationHovered(false)
  , _nSources(dataModel->nPorts(PortType::Out))
  , _nSinks(dataModel->nPorts(PortType::In))
  , _draggingPos(-1000, -1000)
  , _dataModel(dataModel)
  , _fontMetrics(QFont())
  , _boldFontMetrics(QFont())
{
  QFont f; f.setBold(true);

  _boldFontMetrics = QFontMetrics(f);
}

unsigned int
NodeGeometry::nSources() const
{
  return _dataModel->nPorts(PortType::Out);
}

unsigned int
NodeGeometry::nSinks() const
{
  return _dataModel->nPorts(PortType::In);
}

void
NodeGeometry::updatePortCount()
{
	_nSources = _dataModel->nPorts(PortType::Out);
	_nSinks = _dataModel->nPorts(PortType::In);
}

QRectF
NodeGeometry::
entryBoundingRect() const
{
  double const addon = 0.0;

  return QRectF(0 - addon,
                0 - addon,
                _entryWidth + 2 * addon,
                _entryHeight + 2 * addon);
}


QRectF
NodeGeometry::
boundingRect() const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  double addon = 4 * nodeStyle.ConnectionPointDiameter;

  double nameWidth = 0;
  if (!nodeStyle.UseLegacyStyle) {
	  const QString & name = _dataModel->caption();
	  nameWidth = nodeStyle.NameLeftMargin + _fontMetrics.width(name);
  }

  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon + nameWidth,
                _height + 2 * addon);
}


QPainterPath
NodeGeometry::
shape() const
{
	QPainterPath path;
	auto const &nodeStyle = StyleCollection::nodeStyle();

	if (nodeStyle.UseLegacyStyle) {
		path.addRect(boundingRect());
		return path;
	}

	double addon = 4 * nodeStyle.ConnectionPointDiameter;

	//double nameWidth = 0;
	//const QString & name = _dataModel->caption();
	//nameWidth = nodeStyle.NameLeftMargin + _fontMetrics.width(name);
	
	path.addRect(QRectF(
		0 - addon,
		0 - addon,
		_width + 2 * addon,
		_height + 2 * addon
	));

	return path;
}


void
NodeGeometry::
legacyRecalculateSize() const
{
  _entryHeight = _fontMetrics.height();

  {
    unsigned int maxNumOfEntries = std::max(_nSinks, _nSources);
    unsigned int step = _entryHeight + _spacing;
    _height = step * maxNumOfEntries;
  }

  if (auto w = _dataModel->embeddedWidget())
  {
    _height = std::max(_height, static_cast<unsigned>(w->height()));
  }

  _height += captionHeight();

  _inputPortWidth  = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  _width = _inputPortWidth +
           _outputPortWidth +
           2 * _spacing;

  if (auto w = _dataModel->embeddedWidget())
  {
    _width += w->width();
  }

  _width = std::max(_width, captionWidth());

  if (_dataModel->validationState() != NodeValidationState::Valid)
  {
    _width   = std::max(_width, validationWidth());
    _height += validationHeight() + _spacing;
  }
}


void
NodeGeometry::
recalculateSize() const
{
	auto const &nodeStyle = StyleCollection::nodeStyle();
	if (nodeStyle.UseLegacyStyle) {
		legacyRecalculateSize();
		return;
	}

	_entryHeight = _fontMetrics.height();
	_width = 65;
	_height = _width * 0.35;

	if (auto w = _dataModel->embeddedWidget())
	{
		_height = std::max(_height, static_cast<unsigned>(w->height()));
	}
	_height += captionHeight();
}


void
NodeGeometry::
recalculateSize(QFont const & font) const
{
  QFontMetrics fontMetrics(font);
  QFont boldFont = font;

  boldFont.setBold(true);

  QFontMetrics boldFontMetrics(boldFont);

  if (_boldFontMetrics != boldFontMetrics)
  {
    _fontMetrics     = fontMetrics;
    _boldFontMetrics = boldFontMetrics;

    recalculateSize();
  }
}


QPointF
NodeGeometry::
legacyPortScenePosition(PortIndex index,
	PortType portType) const
{
	auto const &nodeStyle = StyleCollection::nodeStyle();

	unsigned int step = _entryHeight + _spacing;

	QPointF result;

	double totalHeight = 0.0;

	totalHeight += captionHeight() + _entryHeight / 2.0;

	totalHeight += step * index;

	switch (portType)
	{
	case PortType::Out:
	{
		double x = _width + nodeStyle.ConnectionPointDiameter;

		result = QPointF(x, totalHeight);
		break;
	}

	case PortType::In:
	{
		double x = 0.0 - nodeStyle.ConnectionPointDiameter;

		result = QPointF(x, totalHeight);
		break;
	}

	default:
		break;
	}

	return result;
}

QPointF
NodeGeometry::
portScenePosition(PortIndex index,
                  PortType portType) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  if (nodeStyle.UseLegacyStyle) {
	  return legacyPortScenePosition(index, portType);
  }

  float diam = nodeStyle.ConnectionPointDiameter;
  int n = _dataModel->nPorts(portType);

  float minSpacing = nodeStyle.ConnectionPointMinimumSpacing;
  float margin = nodeStyle.ConnectionPointMargin;
  float spacing = std::max((_width - n * diam) / (n + 1), minSpacing);
  float totalWidth = spacing * (n + 1) + diam * n;

  unsigned int step = spacing + diam;

  QPointF result;

  double x = spacing + diam / 2 - (totalWidth - _width) / 2;
  x += step * index;

  double y = margin + diam / 2;
  y = portType == PortType::In ? -y : _height + y;

  return QPointF(x, y);
}


PortIndex
NodeGeometry::
checkHitScenePoint(PortType portType,
                   QPointF const scenePoint,
                   QTransform const & sceneTransform) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  if (portType == PortType::None) return INVALID;

  if (!nodeStyle.UseLegacyStyle) {
    // Priority to node body over ports
      QRectF body = sceneTransform.mapRect(QRectF(0, 0, _width, _height));
      if (body.contains(scenePoint)) {
          return INVALID;
      }
  }

  double const tolerance = nodeStyle.ConnectionPointToleranceFactor * nodeStyle.ConnectionPointDiameter;

  unsigned int const nItems = _dataModel->nPorts(portType);

  for (unsigned int i = 0; i < nItems; ++i)
  {
    auto pp = sceneTransform.map(portScenePosition(i, portType));

    QPointF p = pp - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
    {
      return PortIndex(i);
    }
  }

  return INVALID;
}


QRect
NodeGeometry::
resizeRect() const
{
  unsigned int rectSize = 7;

  return QRect(_width - rectSize,
               _height - rectSize,
               rectSize,
               rectSize);
}


QPointF
NodeGeometry::
widgetPosition() const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();
  if (!nodeStyle.UseLegacyStyle) {
	return QPointF(_width, 0);
  }
  if (auto w = _dataModel->embeddedWidget())
  {
    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, place it immediately after the caption.
      return QPointF(_spacing + portWidth(PortType::In), captionHeight());
    }
    else
    {
      if (_dataModel->validationState() != NodeValidationState::Valid)
      {
        return QPointF(_spacing + portWidth(PortType::In),
                      (captionHeight() + _height - validationHeight() - _spacing - w->height()) / 2.0);
      }

      return QPointF(_spacing + portWidth(PortType::In), 
                    (captionHeight() + _height - w->height()) / 2.0);
    }
  }
  return QPointF();
}

int
NodeGeometry::
equivalentWidgetHeight() const
{
  if (_dataModel->validationState() != NodeValidationState::Valid)
  {
    return height() - captionHeight() + validationHeight();
  }

  return height() - captionHeight();
}

unsigned int
NodeGeometry::
captionHeight() const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();
  if (!nodeStyle.UseLegacyStyle) {
    return 0;
  }

  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _boldFontMetrics.boundingRect(name).height();
}


unsigned int
NodeGeometry::
captionWidth() const
{
  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _boldFontMetrics.boundingRect(name).width();
}


unsigned int
NodeGeometry::
validationHeight() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).height();
}


unsigned int
NodeGeometry::
validationWidth() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).width();
}

QRect
NodeGeometry::
validationRect() const
{
	auto const &nodeStyle = StyleCollection::nodeStyle();
	float diam = nodeStyle.ConnectionPointDiameter;
	QSize size(diam, height() - diam);
	return QRect(
		QPoint(-diam / 2 - size.width(), (height() - size.height()) / 2),
		size
	);
}

QPointF
NodeGeometry::
calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node* targetNode, 
                                      PortIndex sourcePortIndex, PortType sourcePort, Node* sourceNode, 
                                      Node& newNode)
{
  //Calculating the nodes position in the scene. It'll be positioned half way between the two ports that it "connects". 
  //The first line calculates the halfway point between the ports (node position + port position on the node for both nodes averaged).
  //The second line offsets this coordinate with the size of the new node, so that the new nodes center falls on the originally
  //calculated coordinate, instead of it's upper left corner.
  auto converterNodePos = (sourceNode->nodeGraphicsObject().pos() + sourceNode->nodeGeometry().portScenePosition(sourcePortIndex, sourcePort) +
    targetNode->nodeGraphicsObject().pos() + targetNode->nodeGeometry().portScenePosition(targetPortIndex, targetPort)) / 2.0f;
  converterNodePos.setX(converterNodePos.x() - newNode.nodeGeometry().width() / 2.0f);
  converterNodePos.setY(converterNodePos.y() - newNode.nodeGeometry().height() / 2.0f);
  return converterNodePos;
}


unsigned int
NodeGeometry::
portWidth(PortType portType) const
{
  unsigned width = 0;

  for (auto i = 0ul; i < _dataModel->nPorts(portType); ++i)
  {
    QString name;

    if (_dataModel->portCaptionVisible(portType, i))
    {
      name = _dataModel->portCaption(portType, i);
    }
    else
    {
      name = _dataModel->dataType(portType, i).name;
    }

    width = std::max(unsigned(_fontMetrics.width(name)),
                     width);
  }

  return width;
}
