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

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}
	
	std::ostringstream ss;
	ss << cmd.cmd << " -vf scale=" << ui->widthInput->value() << ":" << ui->heightInput->value();
	cmd.cmd = ss.str();
	return true;
}

// Data model

int ScaleNode::parmCount() const
{
	return 2;
}

QString ScaleNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "width";
	case 1:
		return "height";
	default:
		return QString();
	}
}

QVariant ScaleNode::parmEval(int parm) const
{
	switch (parm)
	{
	case 0:
		return ui->widthInput->value();
	case 1:
		return ui->heightInput->value();
	default:
		return QVariant();
	}
}

void ScaleNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		ui->widthInput->setValue(value.toInt());
		break;
	case 1:
		ui->heightInput->setValue(value.toInt());
		break;
	}
}

