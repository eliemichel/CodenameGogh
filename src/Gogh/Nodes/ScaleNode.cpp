#include "ScaleNode.h"
#include "ui_ScaleNode.h"

#include "Logger.h"

#include <sstream>

ScaleNode::ScaleNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::ScaleNode)
{
	ui->setupUi(this);
	m_node_name = "Scale";

	// Add slots
	newInputSlot();
	newOutputSlot();
}

ScaleNode::~ScaleNode()
{
}

bool ScaleNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	stringlist pattern;
	return buildRenderCommand(outputIndex, cmd, pattern);
}
bool ScaleNode::buildRenderCommand(int outputIndex, RenderCommand & cmd, stringlist & pattern) const
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
		std::ostringstream ss;
		ss << ui->widthInput->value() << "x" << ui->heightInput->value();
		cmd.cmd.push_back(ss.str());
	}
	else
	{
		cmd.cmd.push_back("-vf");
		std::ostringstream ss;
		ss << "scale=" << ui->widthInput->value() << ":" << ui->heightInput->value();
		cmd.cmd.push_back(ss.str());	
	}
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
