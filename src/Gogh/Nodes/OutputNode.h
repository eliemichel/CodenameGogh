#ifndef H_OUTPUTNODE
#define H_OUTPUTNODE

#include "NodeWidget.h"

namespace Ui {
	class OutputNode;
}

class OutputNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit OutputNode(QWidget *parent = 0);
	~OutputNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	QVariant parmEval(int parm) const override;
	void setParm(int parm, QVariant value) override;

protected:
	void slotConnectEvent(SlotEvent *event) override;

protected:
	std::string m_node_name;	

private slots:
	void render();
	void setUserDefined();

private:
	Ui::OutputNode *ui;

	/// Used to keep track of whether the user set the filename or whether it has been auto generated and must hence be updated
	/// if filename is user-defined, it is displayed in bold
	bool m_isFilenameUserDefined;
};

#endif // H_OUTPUTNODE
