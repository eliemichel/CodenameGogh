#include "MixNode.h"

#include "Logger.h"

#include <sstream>
#include <string>
#include <QDataStream>

MixNode::MixNode()
{
	newInputSlot();
	m_streams.push_back("");
	newInputSlot();
	m_streams.push_back("");

	newOutputSlot();
}

bool MixNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	//Save parent stream name
	std::string parentName;
	if(cmd.names.size() > 0)
	{
		parentName = cmd.names[cmd.names.size()-1];
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	// Map data for every inputSlot
	for (int i = 0; i < parmCount(); i++)
	{
		//Clear current settings
		cmd.cs.clear();

		int id = cmd.outputs.size();

		// Get output stream name
		if (parentName == "")
		{
			cmd.names[id] = parmEvalAsString(i).toStdString();
		} else
		{
			cmd.names[id] = parentName + parmEvalAsString(i).toStdString();
		}

		// Get informations from intputSlot
		if(parentBuildRenderCommand(i, cmd))
		{
			// Add input stream
			cmd.inputs.push_back(cmd.fs);

			// Creates one output stream with the current stream
			cmd.outputs[id] = cmd.fs;

			// Get settings of this output stream
			cmd.settings[id] = cmd.cs;
		}
	}
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
	if (event->isInputSlot() && event->slotIndex() == 0 && parmCount() >= 2)
	{
		newInputSlot();
		m_streams.push_back("");
		emit parmChanged(parmCount() - 1);
	}
}
