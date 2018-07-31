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

// Data model

int OutputNode::parmCount() const
{
	return 1;
}

QString OutputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	default:
		return QString();
	}
}

QVariant OutputNode::parmEval(int parm) const
{
	switch (parm)
	{
	case 0:
		return ui->filenameInput->text();
	default:
		return QVariant();
	}
}

void OutputNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		ui->filenameInput->setText(value.toString());
		break;
	}
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
