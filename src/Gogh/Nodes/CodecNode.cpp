#include "CodecNode.h"
#include "ui_CodecNode.h"

#include "Logger.h"

#include <sstream>

CodecNode::CodecNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::CodecNode)
	, m_node_name("codec")
{
	ui->setupUi(this);

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
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}
	/*int cmdLen = cmd.cmd.size();
	//cmd.cmd.at(cmdLen - 1) = ui->codecBox->currentText().toStdString();
	cmd.cmd.resize(cmdLen + 1, cmd.cmd[cmdLen - 1]);
	cmd.cmd.at(cmdLen - 1) = ui->codecBox->currentText().toStdString();*/
	cmd.keys[m_node_name] = ui->codecBox->currentText().toStdString();

	cmd.cmd.push_back("-c:v");
	cmd.cmd.push_back(ui->codecBox->currentText().toStdString());

	return true;
}
