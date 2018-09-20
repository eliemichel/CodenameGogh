#include "FramerateNode.h"
#include "Parameter.h"
#include "Logger.h"

#include <sstream>

FramerateNode::FramerateNode()
{
	appendInputSlot();
	appendOutputSlot();
	appendParam();

	Parameter & fpsParam = param(0);
	fpsParam.setName("Framerate");
	fpsParam.setType(IntType);
	fpsParam.set(25);
}

bool FramerateNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	//Output smart-renaming
	cmd.env["framerate"] = param(0).evalAsString();

	//RenderCommand
	cmd.os.settings.push_back("-r");
	cmd.os.settings.push_back(param(0).evalAsString());

	return true;
}
