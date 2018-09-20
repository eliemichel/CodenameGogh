#ifndef H_MIXNODE
#define H_MIXNODE

#include "Node.h"

#include <vector>
#include <string>

class MixNode : public Node
{
public:
	MixNode();

public:
	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

protected:
	void inputSlotConnectEvent(SlotEvent *event) override;
};

#endif // H_MIXNODE
