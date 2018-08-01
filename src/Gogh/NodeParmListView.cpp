#include "NodeParmListView.h"
#include "Logger.h"

#include <QGridLayout>
#include <QLabel>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

NodeParmListView::NodeParmListView(QWidget *parent)
	: QWidget(parent)
	, m_model(nullptr)
	, m_selectionModel(nullptr)
{
	QGridLayout *layout = new QGridLayout();
	layout->addWidget(new QLabel("parm"), 0, 0);
	layout->addWidget(new QLabel("value"), 0, 1);
	this->setLayout(layout);
}

void NodeParmListView::setModel(QAbstractItemModel * model)
{
	if (m_model)
	{
		disconnect(m_model, 0, this, 0);
	}
	m_model = model;
	connect(m_model, &QAbstractItemModel::dataChanged, this, &NodeParmListView::onDataChanged);
	updateContent();
}

void NodeParmListView::setSelectionModel(QItemSelectionModel * selectionModel)
{
	if (m_selectionModel)
	{
		disconnect(m_selectionModel, 0, this, 0);
	}
	m_selectionModel = selectionModel;
	connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &NodeParmListView::updateContent);
	updateContent();
}

void NodeParmListView::updateContent()
{
	// clean-up
	QLayoutItem *item;
	while (item = layout()->takeAt(0))
	{
		delete item->widget();
		delete item;
	}
	delete layout();

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(new QLabel("parm"), 0, 0);
	layout->addWidget(new QLabel("value"), 0, 1);
	if (model() && selectionModel())
	{
		m_currentNodeIndex = selectionModel()->currentIndex();
		while (m_currentNodeIndex.isValid() && m_currentNodeIndex.parent().isValid())
		{
			m_currentNodeIndex = m_currentNodeIndex.parent();
		}
		if (m_currentNodeIndex.isValid() && !m_currentNodeIndex.parent().isValid()) // if isNodeIndex
		{
			int n = model()->rowCount(m_currentNodeIndex);;
			for (int i = 0; i < n; ++i)
			{
				QString parm = model()->data(model()->index(i, 0, m_currentNodeIndex), Qt::DisplayRole).toString();
				layout->addWidget(new QLabel(parm), i + 1, 0);

				QVariant value = model()->data(model()->index(i, 1, m_currentNodeIndex), Qt::DisplayRole);
				layout->addWidget(new QLabel(value.toString()), i + 1, 1);
			}
		}
	}
	this->setLayout(layout);
}

void NodeParmListView::onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	bool mustUpdate = false;
	if (!topLeft.isValid() || !bottomRight.isValid() || !m_currentNodeIndex.isValid())
	{
		mustUpdate = true;
	}
	else
	{
		QModelIndex topLeftNode = topLeft;
		while (topLeftNode.parent().isValid())
		{
			topLeftNode = topLeftNode.parent();
		}

		QModelIndex bottomRightNode = bottomRight;
		while (bottomRightNode.parent().isValid())
		{
			bottomRightNode = bottomRightNode.parent();
		}

		mustUpdate = m_currentNodeIndex.row() >= topLeftNode.row() && m_currentNodeIndex.row() <= bottomRightNode.row();
	}

	if (mustUpdate)
	{
		updateContent();
	}
}
