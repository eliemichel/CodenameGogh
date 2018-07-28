#ifndef H_SLOT
#define H_SLOT

#include <vector>

class LinkGraphicsItem;
class NodeWidget;

/**
* Typically a slot has a maxInputs or a maxOutputs set to zero, because it is respectively an output or an input node.
* The exception is for routing nodes. The number of input can never exceed 1.
* The slot references a widget to get its position, so that it is always correct even when complex layout cases occur.
*/
class Slot
{
public:
	Slot() : m_sourceSlot(nullptr) {}

	const std::vector<LinkGraphicsItem*> inputLinks() const { return m_inputLinks; }
	void addInputLink(LinkGraphicsItem *link) { m_inputLinks.push_back(link); }

	const std::vector<LinkGraphicsItem*> outputLinks() const { return m_outputLinks; }
	void addOutputLink(LinkGraphicsItem *link) { m_outputLinks.push_back(link); }

	int maxInputs() const { return m_maxInputs; }
	void setMaxInputs(int maxInputs);

	int maxOutputs() const { return m_maxOutputs; }
	void setMaxOutputs(int maxOutputs) { m_maxOutputs = maxOutputs; }

	const Slot * sourceSlot() const { return m_sourceSlot; }
	void setSourceSlot(Slot *sourceSlot) { m_sourceSlot = sourceSlot; }

	const NodeWidget * parentNode() const { return m_parentNode; }
	void setParentNode(NodeWidget *parentNode) { m_parentNode = parentNode; }

private:
	std::vector<LinkGraphicsItem*> m_inputLinks;
	std::vector<LinkGraphicsItem*> m_outputLinks;
	int m_maxInputs = 1;
	int m_maxOutputs = -1;
	Slot *m_sourceSlot;
	NodeWidget *m_parentNode;
};

#endif // H_SLOT
