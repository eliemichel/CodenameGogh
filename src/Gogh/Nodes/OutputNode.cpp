#include "OutputNode.h"
#include "Ui/NodeDelegate.h"
#include "Parameter.h"
#include "Logger.h"

OutputNode::OutputNode()
	: m_isFilenameUserDefined(false)
{
	appendInputSlot();
	appendParams(4);

	param(0).setName("Filename");
	param(0).setType(StringType);
	
	param(1).setName("Sort streams");
	param(1).setType(CheckboxType);

	param(2).setName("Overwrite");
	param(2).setType(CheckboxType);

	param(3).setName("Render");
	param(3).setType(ButtonType);
}

UiElement * OutputNode::createDelegate(UiLayout *popupLayout) {
	NodeDelegate *del = new NodeDelegate(this, popupLayout);
	del->buttonClicked.connect(this, &OutputNode::onButtonClicked);
	return del;
	// TODO: port OutputNodeEditor to noqt
	//return new OutputNodeEditor(this, parent);
}

bool OutputNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	// Clear cmd --
	// Clear cmd.env and initialize with empty string :
	cmd.env["path"];
	cmd.env["filename"];
	cmd.env["ext"];
	cmd.env["input"];
	cmd.env["scale"];
	cmd.env["codec"];

	// Clear cmd datas
	cmd.sources.clear();
	cmd.outputs.clear();

	// special output index for render function
	if (outputIndex != -1) {
		ERR_LOG << "Invalid output index: #" << outputIndex;
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	if (param(1).evalAsBool())
	{
		cmd.sort_outputs();
	}

	// If there is only one stream to render (no MixNode in the graph)
	if (cmd.outputs.size() == 0)
	{
		cmd.outputs[0] = cmd.os;
	}

	//Build RenderCommand
	int count = static_cast<int>(cmd.outputs.size());

	//Input files
	for (int i = 0; i < cmd.sources.size(); i++) {
		cmd.cmd.push_back("-i");
		cmd.cmd.push_back(cmd.outputs[i].input.first);
	}

	//Files' streams mapping
	for (int i = 0; i < count; i++) {
		cmd.os = cmd.outputs[i];
		cmd.cmd.push_back("-map");
		std::ostringstream ss;
		ss << cmd.source_id(cmd.os.input) << ":" << cmd.os.input.second;
		cmd.cmd.push_back(ss.str());
	}

	//Correct RenderCommand parameters depending on the mapping
	int videoStreamN = 0;
	int audioStreamN = 0;
	int subtitleStreamN = 0;
	int dataStreamN = 0;

	for (int i = 0; i < count; i++)
	{
		cmd.os = cmd.outputs[i];
		//Get the counter for the current stream
		int* currentStreamN = &videoStreamN;
		switch (cmd.os.stream) {
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

		// Copy the stream if no codec modification is set
		if (cmd.os.settings.size() == 0)
		{
			std::ostringstream ss;
			ss << "-c:" << streamTypeAsChar(cmd.os.stream);
			cmd.cmd.push_back(ss.str());
			cmd.cmd.push_back("copy");
		} else
		{
			//Writing settings to output streams
			for (auto& set : cmd.os.settings)
			{
				//Codec : stream type
				std::string codecStreamType = "-c:" + streamTypeAsChar(cmd.os.stream);
				//Correct name based on mapping
				if (set == codecStreamType || set == "-c:b")
				{
					set = set + ":" + std::to_string(*currentStreamN);
				}
				cmd.cmd.push_back(set);
			}
		}

		//Streams Naming
		std::ostringstream ss;
		ss << "-metadata:s:" << streamTypeAsChar(cmd.os.stream) << ":" << *currentStreamN;
		cmd.cmd.push_back(ss.str());
		ss.str(std::string());
		ss << "title=\"" << cmd.os.name << "\"";
		cmd.cmd.push_back(ss.str());

		*currentStreamN += 1;
	}

	//Overwrite the output file
	if (param(2).evalAsBool())
	{
		cmd.cmd.push_back("-y");
	} else
	{
		cmd.cmd.push_back("-n");
	}

	//Write the output file
	cmd.cmd.push_back(param(0).evalAsString());

	return true;
}

void OutputNode::inputSlotConnectEvent(SlotEvent *event)
{
	if (event->slotIndex() == 0)
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
			param(0).set(userPattern);
		}
	}
}

void OutputNode::onButtonClicked(int parm)
{
	switch (parm)
	{
	case 3:
	{
		RenderCommand cmd;
		if (buildRenderCommand(-1, cmd))
		{
			DEBUG_LOG << cmd.cmd;
		}
		break;
	}
	}
}
