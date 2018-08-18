#include "FramerateNode.h"

#include "Logger.h"

#include <sstream>

FramerateNode::FramerateNode()
	: m_framerate(25)
{
	newInputSlot();
	newOutputSlot();
}

bool FramerateNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	//Output smart-renaming
	cmd.env["framerate"] = parmEvalAsString(0).toStdString();

	//RenderCommand
	cmd.cmd.push_back("-r");
	cmd.cmd.push_back(parmEvalAsString(0).toStdString());

	return true;
}

// Data model

int FramerateNode::parmCount() const
{
	return 1;
}

QString FramerateNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "width";
	default:
		return QString();
	}
}

ParmType FramerateNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return IntType;
	case 1:
		return IntType;
	default:
		return NoneType;
	}
}

QVariant FramerateNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return m_framerate;
	default:
		return QVariant();
	}
}

bool FramerateNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		m_framerate = value.toInt();
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}
