#ifndef H_NODEPARMLISTVIEW
#define H_NODEPARMLISTVIEW

#include <QAbstractItemView>

class NodeParmListView : public QAbstractItemView
{
public:
	QModelIndex indexAt(const QPoint & point) const override;
	void scrollTo(const QModelIndex & index, QAbstractItemView::ScrollHint hint = EnsureVisible) override;
	QRect visualRect(const QModelIndex & index) const override;

protected:
	int horizontalOffset() const override;
	bool isIndexHidden(const QModelIndex & index) const override;
	void setSelection(const QRect & rect, QItemSelectionModel::SelectionFlags flags) override;
	int verticalOffset() const override;
	QRegion visualRegionForSelection(const QItemSelection & selection) const override;
};

#endif // H_NODEPARMLISTVIEW
