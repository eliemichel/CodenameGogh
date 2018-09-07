#include "CodecNode.h"

#include "Logger.h"

#include <sstream>

CodecNode::CodecNode()
	: m_codec(0)
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

	//Output smart-renaming
	cmd.env["codec"] = parmEvalAsString(0).toStdString();

	//RenderCommand
	cmd.cs.push_back("-c:v");
	cmd.cs.push_back(parmEvalAsString(0).toStdString());

	if (parmEvalAsBool(1))
	{
		cmd.cs.push_back("-movflags");
		cmd.cs.push_back("faststart");
	}

	return true;
}

// Data model

int CodecNode::parmCount() const
{
	return 2;
}

QString CodecNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "codec";
	case 1:
		return "Web Optimized";
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
	case 1:
		return CheckboxType;
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
	case 1:
		return bool(m_weboptimized);
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
	case 1:
		m_weboptimized = value.toBool();
		emit parmChanged(parm);
		return true;
	default:
		return false;
	}
}
