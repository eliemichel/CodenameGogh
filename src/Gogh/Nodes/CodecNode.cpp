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

	std::ostringstream ss;
 	ss << "-c:v "<< ui->codecBox->currentText().toStdString();
	//std::cout << ui->codecBox->currentText();
	cmd.cmd.push_back(ss.str());
	return true;
}
