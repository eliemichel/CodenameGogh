#ifndef H_SCALENODE
#define H_SCALENODE

#include "Node.h"

class ScaleNode : public Node
{
public:
	ScaleNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;
};

#endif // H_SCALENODE
