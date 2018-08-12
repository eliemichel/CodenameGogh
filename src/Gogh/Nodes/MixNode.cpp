#include "MixNode.h"

#include "Logger.h"

#include <sstream>
#include <string>
#include <QDataStream>

MixNode::MixNode()
{
	newInputSlot();
	newInputSlot();
	newOutputSlot();

	m_streams.push_back("");
}

bool MixNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	//cmd.cmd.push_back("-c:v");
	//cmd.cmd.push_back(parmEvalAsString(0).toStdString());
	return true;
}

// Data model

int MixNode::parmCount() const
{
	return static_cast<int>(m_streams.size());
}

QString MixNode::parmName(int parm) const
{
	if (parm >= 0 && parm < parmCount())
	{
		return "streamName" + QString(parm);
	}
	else
	{
		return QString();
	}
}

ParmType MixNode::parmType(int parm) const
{
	if (parm >= 0 && parm < parmCount())
	{
		return StringType;
	}
	else
	{
		return NoneType;
	}
}

QVariant MixNode::parmRawValue(int parm) const
{
	if (parm >= 0 && parm < parmCount())
	{
		return QString::fromStdString(m_streams[parm]);
	}
	else
	{
		return QVariant();
	}
}

bool MixNode::setParm(int parm, QVariant value)
{
	if (parm >= 0 && parm < parmCount())
	{
		m_streams[parm] = value.toString().toStdString();
		emit parmChanged(parm);
		return true;
	}
	else
	{
		return false;
	}
}

void MixNode::read(QDataStream & stream)
{
	int n;
	stream >> n;
	for (int i = parmCount(); i < n; ++i)
	{
		newInputSlot();
	}
	m_streams.resize(n);
	Node::read(stream);
}

void MixNode::write(QDataStream & stream) const
{
	int n = parmCount();
	stream << n;
	Node::write(stream);
}

void MixNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		newInputSlot();
		m_streams.push_back("");
		emit parmChanged(parmCount() - 1);
	}
}
