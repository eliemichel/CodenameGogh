#include "NodeParmListView.h"

QModelIndex NodeParmListView::indexAt(const QPoint & point) const
{
	return currentIndex();
}

void NodeParmListView::scrollTo(const QModelIndex & index, QAbstractItemView::ScrollHint hint)
{
}

QRect NodeParmListView::visualRect(const QModelIndex & index) const
{
	if (index == currentIndex())
	{
		return rect();
	}
	else
	{
		return QRect();
	}
}

int NodeParmListView::horizontalOffset() const
{
	return 0;
}

bool NodeParmListView::isIndexHidden(const QModelIndex & index) const
{
	return index != currentIndex();
}

void NodeParmListView::setSelection(const QRect & rect, QItemSelectionModel::SelectionFlags flags)
{
	selectionModel()->select(currentIndex(), flags);
}

int NodeParmListView::verticalOffset() const
{
	return 0;
}

QRegion NodeParmListView::visualRegionForSelection(const QItemSelection & selection) const
{
	return rect();
}
