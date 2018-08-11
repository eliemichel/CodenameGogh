#include "CodecNode.h"
#include "ui_CodecNode.h"

#include "Logger.h"

#include <sstream>

CodecNode::CodecNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::CodecNode)
{
	ui->setupUi(this);
	m_node_name = "Codec";

	// Add slots
	newInputSlot();
	newOutputSlot();

	//Add items in combo box
	ui->codecBox->addItem("libx264");
	ui->codecBox->addItem("hap");
}

CodecNode::~CodecNode()
{
}

bool CodecNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	stringlist pattern;
	return buildRenderCommand(outputIndex, cmd, pattern);
}
bool CodecNode::buildRenderCommand(int outputIndex, RenderCommand & cmd, stringlist & pattern) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd, pattern))
	{
		return false;
	}
	if(isPatterned(pattern))
	{
		int cmdLen = cmd.cmd.size();
		//cmd.cmd.at(cmdLen - 1) = ui->codecBox->currentText().toStdString();
		cmd.cmd.resize(cmdLen + 1, cmd.cmd[cmdLen - 1]);
		cmd.cmd.at(cmdLen - 1) = ui->codecBox->currentText().toStdString();
	}
	else
	{
		cmd.cmd.push_back("-c:v");
		cmd.cmd.push_back(ui->codecBox->currentText().toStdString());
	}
	return true;
}
