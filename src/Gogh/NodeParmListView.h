#ifndef H_NODEPARMLISTVIEW
#define H_NODEPARMLISTVIEW

#include <QWidget>
#include <QModelIndex>

class QAbstractItemModel;
class QItemSelectionModel;

class NodeParmListView : public QWidget
{
	Q_OBJECT
public:
	explicit NodeParmListView(QWidget *parent = nullptr);

	QAbstractItemModel * model() const { return m_model; }
	void setModel(QAbstractItemModel *model);

	QItemSelectionModel * selectionModel() const { return m_selectionModel; }
	void setSelectionModel(QItemSelectionModel *selectionModel);

private slots:
	void updateContent();
	void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
	QAbstractItemModel * m_model;
	QItemSelectionModel * m_selectionModel;
	QModelIndex m_currentNodeIndex;
};

#endif // H_NODEPARMLISTVIEW
