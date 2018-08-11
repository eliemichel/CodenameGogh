#include "CodecNode.h"

#include "Logger.h"

#include <sstream>

CodecNode::CodecNode()
	: m_codec(0)
	, m_node_name("codec")
{
	newInputSlot();
	newOutputSlot();
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
	
	cmd.keys[m_node_name] = parmEvalAsString(0).toStdString();

	cmd.cmd.push_back("-c:v");
	cmd.cmd.push_back(parmEvalAsString(0).toStdString());
	return true;
}

// Data model

int CodecNode::parmCount() const
{
	return 1;
}

QString CodecNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "codec";
	default:
		return QString();
	}
}

ParmType CodecNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return EnumType;
	default:
		return NoneType;
	}
}

QVariant CodecNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return m_codec;
	default:
		return QVariant();
	}
}

int CodecNode::parmMenuCount(int parm) const
{
	switch (parm)
	{
	case 0:
		return 2;
	default:
		return 0;
	}
}

QString CodecNode::parmMenuLabel(int parm, int menu) const
{
	switch (menu)
	{
	case 0:
		return "libx264";
	case 1:
		return "hap";
	default:
		return QString();
	}
}

QVariant CodecNode::parmMenuValue(int parm, int menu) const
{
	switch (menu)
	{
	case 0:
		return "libx264";
	case 1:
		return "hap";
	default:
		return QVariant();
	}
}

bool CodecNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		m_codec = value.toInt();
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}
