#include "ScaleNode.h"
#include "ui_ScaleNode.h"

#include "Logger.h"

#include <sstream>

ScaleNode::ScaleNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::ScaleNode)
{
	ui->setupUi(this);

	// Add slots
	newInputSlot();
	newOutputSlot();
}

ScaleNode::~ScaleNode()
{
}

bool ScaleNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	const Slot *sourceSlot = inputSlots()[0]->sourceSlot();
	if (!sourceSlot)
	{
		ERR_LOG << "[ScaleNode] Input 0 is not connected, unable to render";
		return false;
	}
	const NodeWidget *parentNode = sourceSlot->parentNode();
	if (!parentNode)
	{
		ERR_LOG << "[ScaleNode] Input 0 is not connected to an orphan slot";
		return false;
	}

	if (!parentNode->buildRenderCommand(sourceSlot, cmd))
	{
		return false;
	}
	
	std::ostringstream ss;
	ss << cmd.cmd << " -vf scale=" << ui->widthInput->value() << ":" << ui->heightInput->value();
	cmd.cmd = ss.str();
	return true;
}
