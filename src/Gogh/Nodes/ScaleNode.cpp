#include "ScaleNode.h"

#include "Logger.h"

#include <sstream>

ScaleNode::ScaleNode()
	: m_width(1920)
	, m_height(1080)
{
	newInputSlot();
	newOutputSlot();
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

	cmd.cmd.push_back("-vf");
	std::ostringstream ss;
	ss << "scale=" << parmEvalAsInt(0) << ":" << parmEvalAsInt(1);
	cmd.cmd.push_back(ss.str());
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

ParmType ScaleNode::parmType(int parm) const
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

QVariant ScaleNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return m_width;
	case 1:
		return m_height;
	default:
		return QVariant();
	}
}

bool ScaleNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		m_width = value.toInt();
		emit parmChanged(parm);
		return true;
	case 1:
		m_height = value.toInt();
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}
