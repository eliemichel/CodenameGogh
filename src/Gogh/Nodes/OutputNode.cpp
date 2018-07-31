#include "OutputNode.h"
#include "ui_OutputNode.h"

#include "RenderDialog.h"
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

	cmd.cmd = cmd.cmd + " " + ui->filenameInput->text().toStdString();
	return true;
}

void OutputNode::read(QDataStream & stream)
{
	QString filename;
	stream >> filename;
	ui->filenameInput->setText(filename);
}

void OutputNode::write(QDataStream & stream) const
{
	QString filename = ui->filenameInput->text();
	stream << filename;
}

void OutputNode::render()
{
	RenderCommand cmd;
	if (buildRenderCommand(-1, cmd))
	{
		DEBUG_LOG << "Render command: ffmpeg " << cmd.cmd;
		RenderDialog renderDialog(cmd.cmd);
		renderDialog.exec();
	}
	else
	{
		ERR_LOG << "Render failed";
		QMessageBox errDialog;
		errDialog.setText("Render failed");
		errDialog.setInformativeText(QString::fromStdString(cmd.err));
		errDialog.setIcon(QMessageBox::Critical);
		errDialog.exec();
	}
}
