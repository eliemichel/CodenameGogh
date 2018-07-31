#include "NodeParmListView.h"

#include <QGridLayout>
#include <QLabel>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

NodeParmListView::NodeParmListView(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout *layout = new QGridLayout();
	layout->addWidget(new QLabel("parm"), 0, 0);
	layout->addWidget(new QLabel("value"), 0, 1);
	this->setLayout(layout);
}

void NodeParmListView::setModel(QAbstractItemModel * model)
{
	m_model = model;
	// TODO: listen to data change
}

void NodeParmListView::setSelectionModel(QItemSelectionModel * selectionModel)
{
	m_selectionModel = selectionModel;
	connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &NodeParmListView::updateContent);
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
		const QModelIndex & index = selectionModel()->currentIndex();
		if (index.isValid() && !index.parent().isValid()) // if isNodeIndex
		{
			int n = model()->rowCount(index);;
			for (int i = 0; i < n; ++i)
			{
				QString parm = model()->data(model()->index(i, 0, index), Qt::DisplayRole).toString();
				layout->addWidget(new QLabel(parm), i + 1, 0);

				QVariant value = model()->data(model()->index(i, 1, index), Qt::DisplayRole);
				layout->addWidget(new QLabel(value.toString()), i + 1, 1);
			}
		}
	}
	this->setLayout(layout);
}
