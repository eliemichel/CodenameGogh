#include "ScaleNode.h"
#include "Parameter.h"
#include "Logger.h"

#include <sstream>

ScaleNode::ScaleNode() {
	appendInputSlot();
	appendOutputSlot();
	appendParams(2);

	Parameter & widthParam = param(0);
	widthParam.setName("width");
	widthParam.setType(IntType);
	widthParam.set(1920);

	Parameter & heightParam = param(1);
	heightParam.setName("height");
	heightParam.setType(IntType);
	heightParam.set(1080);
}

bool ScaleNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	//Output smart-renaming
	std::ostringstream ss;
	ss << param(0).evalAsInt() << "x" << param(1).evalAsInt();
	cmd.env["scale"] = ss.str();

	//RenderCommand
	cmd.os.settings.push_back("-vf");
	std::ostringstream().swap(ss);
	ss << "scale=" << param(0).evalAsInt() << ":" << param(1).evalAsInt();
	cmd.os.settings.push_back(ss.str());

	return true;
}
