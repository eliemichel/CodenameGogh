#ifndef H_CODECNODE
#define H_CODECNODE

#include "Node.h"

class CodecNode : public Node
{
public:
	CodecNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;
};

#endif // H_CODECNODE
