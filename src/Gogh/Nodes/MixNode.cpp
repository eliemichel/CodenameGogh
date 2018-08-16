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

	// Clear cmd
	//std::vector<std::string>().swap(cmd);
	cmd.cmd.clear();

	// Map inputs
	stringlist inputFiles;
	int currentFileID = 0;

	for (int i = 0; i < parmCount(); i++)
	{
		RenderCommand cmx;
		parentBuildRenderCommand(i, cmx);
		bool isNewFile = true;
		for (int j = 0; j <= inputFiles.size(); j++)
		{
			if (inputFiles.size() > 0 && cmx.cmd[1] == inputFiles[j])
			{
				isNewFile = false;
				currentFileID = j;
				break;
			}
		}
		if (isNewFile)
		{
			inputFiles.push_back(cmx.cmd[1]);
			currentFileID = i;
		}

		//Build command
		cmd.cmd.push_back("-i");
		cmd.cmd.push_back(inputFiles[i]);
		cmd.cmd.push_back("-map");
		cmd.cmd.push_back(std::to_string(currentFileID));
		cmd.cmd.push_back(":");
		cmd.cmd.push_back(std::to_string(cmx.map));
	}

	
	/*for (int i = 0; i < cmd.cmd.size(); i++)
	{
		ss << cmd.cmd[i];
	}*/

	//DEBUG_LOG << cmd.cmd;

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
