#ifndef H_FRAMERATENODE
#define H_FRAMERATENODE

#include "Node.h"

class FramerateNode : public Node
{
public:
	FramerateNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;
};

#endif // H_FRAMERATENODE
