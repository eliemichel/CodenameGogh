#include "CodecNode.h"
#include "ui_CodecNode.h"

#include "Logger.h"

#include <sstream>

CodecNode::CodecNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::CodecNode)
{
	ui->setupUi(this);

	// Add slots
	newInputSlot();
	newOutputSlot();
}

CodecNode::~CodecNode()
{
}

bool CodecNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	std::ostringstream ss;
	ss << cmd.cmd << " -vf scale=" << ui->widthInput->value() << ":" << ui->heightInput->value();
	cmd.cmd = ss.str();
	return true;
}
