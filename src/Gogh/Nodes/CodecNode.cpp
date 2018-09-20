#include "CodecNode.h"
#include "Parameter.h"
#include "Logger.h"

#include <sstream>

CodecNode::CodecNode() {
	appendInputSlot();
	appendOutputSlot();
	appendParams(2);

	Parameter & codecParam = param(0);
	codecParam.setName("Codec");
	codecParam.setType(EnumType);
	codecParam.appendMenuItems(2);
	codecParam.setMenuLabel(0, "libx264");
	codecParam.setMenuValue(0, "libx264");
	codecParam.setMenuLabel(1, "hap");
	codecParam.setMenuValue(1, "hap");
	codecParam.set(0);

	Parameter & webOptimParam = param(1);
	webOptimParam.setName("Web optimized");
	webOptimParam.setType(CheckboxType);
	webOptimParam.set(false);
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
	cmd.env["codec"] = param(0).evalAsString();

	//RenderCommand
	std::ostringstream ss;
	ss << "-c:" << streamTypeAsChar(cmd.os.stream);
	cmd.os.settings.push_back(ss.str());
	cmd.os.settings.push_back(param(0).evalAsString());

	if (param(1).evalAsBool())
	{
		cmd.os.settings.push_back("-movflags");
		cmd.os.settings.push_back("faststart");
	}

	return true;
}
