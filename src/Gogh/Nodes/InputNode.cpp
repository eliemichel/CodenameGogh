#include "InputNode.h"
#include "ui_InputNode.h"

#include "Logger.h"

InputNode::InputNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::InputNode)
{
	ui->setupUi(this);

	// Add slots
	newOutputSlot();
}

InputNode::~InputNode()
{
}

bool InputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	cmd.cmd = "-i " + ui->filenameInput->text().toStdString();
	return true;
}
