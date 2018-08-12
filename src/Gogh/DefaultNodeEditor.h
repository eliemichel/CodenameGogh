#ifndef H_DEFAULTNODEEDITOR
#define H_DEFAULTNODEEDITOR

#include <QWidget>
#include <vector>

class Node;
class QGridLayout;

class DefaultNodeEditor : public QWidget
{
	Q_OBJECT
public:
	explicit DefaultNodeEditor(Node *node, QWidget *parent = nullptr);

	QWidget * inputWidget(int parm) { return parm >= 0 && parm < m_inputs.size() ? m_inputs[parm] : nullptr; }

protected:
	Node * node() const { return m_node; }

signals:
	void buttonClicked(int parm);

private slots:
	void updateParm(int parm);

private:
	void addParmInput(int parm);

private:
	QGridLayout *m_layout;
	std::vector<QWidget*> m_inputs;
	Node *m_node;
};

#endif // H_DEFAULTNODEEDITOR
