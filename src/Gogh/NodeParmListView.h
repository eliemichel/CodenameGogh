#ifndef H_NODEPARMLISTVIEW
#define H_NODEPARMLISTVIEW

#include <QWidget>

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

private:
	QAbstractItemModel * m_model;
	QItemSelectionModel * m_selectionModel;
};

#endif // H_NODEPARMLISTVIEW
