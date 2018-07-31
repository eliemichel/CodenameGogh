#ifndef H_INPUTNODE
#define H_INPUTNODE

#include "NodeWidget.h"

namespace Ui {
	class InputNode;
}

class InputNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit InputNode(QWidget *parent = 0);
	~InputNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	QVariant parmEval(int parm) const override;
	void setParm(int parm, QVariant value) override;

private:
	Ui::InputNode *ui;
};

#endif // H_INPUTNODE
