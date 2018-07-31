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

	void read(QDataStream & stream) override;
	void write(QDataStream & stream) const override;

private:
	Ui::InputNode *ui;
};

#endif // H_INPUTNODE
