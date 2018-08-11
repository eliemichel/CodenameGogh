#ifndef H_OUTPUTNODEEDITOR
#define H_OUTPUTNODEEDITOR

#include <QWidget>
#include <vector>

class OutputNode;
class QLineEdit;

class OutputNodeEditor : public QWidget
{
	Q_OBJECT
public:
	explicit OutputNodeEditor(OutputNode *node, QWidget *parent = nullptr);

private:
	OutputNode * node() const { return m_node; }

private slots:
	void render();
	void setUserDefined();

private:
	OutputNode *m_node;

	/// Used to keep track of whether the user set the filename or whether it has been auto generated and must hence be updated
	/// if filename is user-defined, it is displayed in bold
	bool m_isFilenameUserDefined;

	QLineEdit *m_filenameInput;
};


#endif // H_OUTPUTNODEEDITOR
