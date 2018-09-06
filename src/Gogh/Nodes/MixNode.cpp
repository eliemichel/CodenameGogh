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
	newInputSlot();
	m_streams.push_back("");

	newOutputSlot();
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

	// Clear cmd.cmd
	//cmd.cmd.clear();

	// Map inputs
	//stringlist cmd.inputs;
	std::vector<StreamType> parmStreams;
	std::vector<stringlist> parmCommands;
	int currentFileID = 0;
	std::map<std::string, std::vector<int>> fileMaps;

	for (int i = 0; i < parmCount(); i++)
	{
		//Clear current settings
		cmd.cs.clear();

		//RenderCommand cmd;
		parentBuildRenderCommand(i, cmd);

		// Creates one output stream for every inputSlot
		cmd.outputs[cmd.outputs.size()] = cmd.fs;

		//Get every input file and if it already exists, get the ID
		bool isNewFile = true;
		for (int j = 0; j < cmd.inputs.size(); j++)
		{
			if (cmd.inputs.size() > 0 && cmd.fs == cmd.inputs[j])
			{
				isNewFile = false;
				currentFileID = j;
				break;
			}
		}
		if (isNewFile)
		{
			cmd.inputs.push_back(cmd.fs);
			currentFileID = i;
			isNewFile = false;
		}

		//Get settings of this output stream
		cmd.settings[cmd.outputs.size()] = cmd.cs;

		//Get output stream name
		cmd.names[cmd.outputs.size()] = parmEvalAsString(i).toStdString();
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
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		return;
		newInputSlot();
		m_streams.push_back("");
		emit parmChanged(parmCount() - 1);
	}
}
