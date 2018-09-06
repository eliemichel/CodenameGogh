#include "OutputNode.h"

#include "OutputNodeEditor.h"
#include "Logger.h"

OutputNode::OutputNode()
	: m_isFilenameUserDefined(false)
{
	// Add slots
	newInputSlot();
}

QWidget * OutputNode::createEditor(QWidget * parent)
{
	return new OutputNodeEditor(this, parent);
}

bool OutputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	// Clean cmd.env :
	cmd.env["path"];
	cmd.env["filename"];
	cmd.env["ext"];
	cmd.env["input"];
	cmd.env["scale"];
	cmd.env["codec"];

	// special output index for render function
	if (outputIndex != -1) {
		ERR_LOG << "Invalid output index: #" << outputIndex;
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	if(parmEvalAsBool(1))
	{
		//cmd.cmd.push_back("-movflags");
		//cmd.cmd.push_back("faststart");
	}
	//cmd.cmd.push_back(parmEvalAsString(0).toStdString());

	//Build RenderCommand
	int count = cmd.outputs.size();

	//Input files
	for (size_t i = 0; i < count; i++) {
		cmd.cmd.push_back("-i");
		cmd.cmd.push-back(cmd.inputs[i].first);
	}

	//Files' streams mapping
	for (size_t i = 0; i < count; i++) {
		cmd.cmd.push_back("-map");
		std::ostringstream ss;
		ss << cmd.input_id(cmd.inputs[i].first) << ":" << cmd.inputs[i].second;
		DEBUG_LOG << ss;
	}

	currentFileID = 0;
	for (auto const& m : fileMaps)
	{
		for (int i = 0; i < fileMaps[m.first].size(); i++)
		{
			//Mapping
			cmd.cmd.push_back("-map");
			std::ostringstream ss;
			ss << currentFileID << ":" << fileMaps[m.first][i];
			cmd.cmd.push_back(ss.str());
		}
		currentFileID++;
	}

	//Correct RenderCommand parameters depending on the mapping
	int videoStreamN = 0;
	int audioStreamN = 0;
	int subtitleStreamN = 0;
	int dataStreamN = 0;

	for (int i = 0; i < parmCount(); i++)
	{
		//Get the counter for the current stream
		int* currentStreamN = &videoStreamN;
		switch (parmStreams[i]) {
			case VideoStream:
				currentStreamN = &videoStreamN;
				break;
			case AudioStream:
				currentStreamN = &audioStreamN;
				break;
			case SubtitleStream:
				currentStreamN = &subtitleStreamN;
				break;
			case DataStream:
				currentStreamN = &dataStreamN;
				break;
			default:
				DEBUG_LOG << "This stream type doesn't exit.";
				break;
		}
		//Correct name based on mapping
		for (auto& pc : parmCommands[i])
		{
			if (pc == "-c:v" || pc == "-c:b")
			{
				pc = pc + ":" + std::to_string(*currentStreamN);
			}
		}
		//Streams Naming
		std::ostringstream ss;
		ss << "-metadata:s:" << parmStreams[i] << ":" << *currentStreamN;
		cmd.cmd.push_back(ss.str());
		ss.str(std::string());
		ss << "title=\"" << parmEvalAsString(i).toStdString() << "\"";
		cmd.cmd.push_back(ss.str());

		*currentStreamN += 1;

		//Final build of RenderCommand
		cmd.cmd.insert(std::end(cmd.cmd), std::begin(parmCommands[i]), std::end(parmCommands[i]));
	}
	return true;
}

// Data model

int OutputNode::parmCount() const
{
	return 3;
}

QString OutputNode::parmName(int parm) const
{
	switch (parm)
	{
	case 0:
		return "filename";
	case 1:
		return "Web Optimized";
	case 2:
		return "Render";
	default:
		return QString();
	}
}

ParmType OutputNode::parmType(int parm) const
{
	switch (parm)
	{
	case 0:
		return StringType;
	case 1:
		return CheckboxType;
	case 2:
		return ButtonType;
	default:
		return NoneType;
	}
}

QVariant OutputNode::parmRawValue(int parm) const
{
	switch (parm)
	{
	case 0:
		return QString::fromStdString(m_filename);
	case 1:
		return bool(m_weboptimized);
	default:
		return QVariant();
	}
}

bool OutputNode::setParm(int parm, QVariant value)
{
	switch (parm)
	{
	case 0:
		m_filename = value.toString().toStdString();
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

void OutputNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		if (!m_isFilenameUserDefined)
		{
			// Smart ouput renaming
			RenderCommand cmd;
			//TODO: userPattern from user_preferences menu
			std::string userPattern = "$path$filename_$codec_$scale.$ext";

			if (buildRenderCommand(-1, cmd))
			{
				for (auto const& e : cmd.env)
				{
					replace(userPattern, "$" + e.first, cmd.env[e.first]);
				}
			}
			setParm(0, QString().fromStdString(userPattern));
		}
	}
}
