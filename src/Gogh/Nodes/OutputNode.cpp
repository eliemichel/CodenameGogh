#include "OutputNode.h"
#include "ui_OutputNode.h"

#include "Logger.h"

#include <QMessageBox>

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
	// special output index for render function
	if (outputIndex != -1) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
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
		QMessageBox msgBox;
		msgBox.setText("Render failed");
		msgBox.setInformativeText(QString::fromStdString(cmd.err));
		msgBox.setIcon(QMessageBox::Critical);
		int ret = msgBox.exec();
		ERR_LOG << "Render failed";
	}
}
