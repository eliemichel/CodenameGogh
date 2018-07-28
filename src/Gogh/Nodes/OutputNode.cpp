#include "OutputNode.h"
#include "ui_OutputNode.h"

#include "Logger.h"

OutputNode::OutputNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::OutputNode)
{
	ui->setupUi(this);

	connect(ui->renderButton, &QPushButton::clicked, this, &OutputNode::render);

	// Add slots
	newInputSlot();
}

OutputNode::~OutputNode()
{
}

bool OutputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != -1) {
		return false;
	}

	const Slot *sourceSlot = inputSlots()[0]->sourceSlot();
	if (!sourceSlot)
	{
		ERR_LOG << "[OutputNode] Input 0 is not connected, unable to render";
		return false;
	}
	const NodeWidget *parentNode = sourceSlot->parentNode();
	if (!parentNode)
	{
		ERR_LOG << "[OutputNode] Input 0 is not connected to an orphan slot";
		return false;
	}

	if (!parentNode->buildRenderCommand(sourceSlot, cmd))
	{
		return false;
	}

	cmd.cmd = "ffmpeg " + cmd.cmd + " " + ui->filenameInput->text().toStdString();
	return true;
}

void OutputNode::render()
{
	RenderCommand cmd;
	if (buildRenderCommand(-1, cmd))
	{
		DEBUG_LOG << "Render command: " << cmd.cmd;
	}
	else
	{
		ERR_LOG << "Render failed";
	}
}
