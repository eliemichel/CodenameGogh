#include "MixNode.h"
#include "Parameter.h"
#include "Logger.h"

#include <sstream>
#include <string>

MixNode::MixNode()
{
	appendInputSlots(2);
	appendOutputSlot();
	appendParams(2);

	param(0).setName("Stream Name #0");
	param(0).setType(StringType);
	param(1).setName("Stream Name #1");
	param(1).setType(StringType);
}

bool MixNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	//Save parent stream name
	std::string parentName;
	if (cmd.outputs.size() > 0)
	{
		parentName = cmd.os.name;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	// Map data for every inputSlot
	for (int i = 0; i < paramCount(); i++)
	{
		// Get output stream name
		if (parentName == "")
		{
			cmd.os.name = param(i).evalAsString();
		} else
		{
			cmd.os.name = parentName + param(i).evalAsString();
		}

		//Save outputs count for output creation test: if outputsCount != outputs.size(),
		//then this inputSlot is chained to a MixNode, don't create output for this one.
		int outputsCount = static_cast<int>(cmd.outputs.size());

		// Clear settings
		cmd.os.settings.clear();

		// Get informations from intputSlot
		if (parentBuildRenderCommand(i, cmd) && outputsCount == cmd.outputs.size())
		{
			int id = static_cast<int>(cmd.outputs.size());
			// Creates one output stream with the current stream
			cmd.outputs[id] = cmd.os;
		}
	}
	return true;
}

void MixNode::inputSlotConnectEvent(SlotEvent *event)
{
	if (event->slotIndex() == 0 && paramCount() >= 2)
	{
		appendInputSlot();
		appendParam();
	}
}
