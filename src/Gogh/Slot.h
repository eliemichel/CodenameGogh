#ifndef H_SLOT
#define H_SLOT

#include <vector>

class SlotGraphicsItem;
class NodeWidget;

/**
* Typically a slot has a maxInputs or a maxOutputs set to zero, because it is respectively an output or an input node.
* The exception is for routing nodes. The number of input can never exceed 1.
* The slot references a widget to get its position, so that it is always correct even when complex layout cases occur.
*/
class Slot
{
public:
	Slot() : m_sourceSlot(nullptr), m_parentNode(nullptr), m_graphicItem(nullptr){}

	int maxInputs() const { return m_maxInputs; }
	void setMaxInputs(int maxInputs);

	int maxOutputs() const { return m_maxOutputs; }
	void setMaxOutputs(int maxOutputs) { m_maxOutputs = maxOutputs; }

	const Slot * sourceSlot() const { return m_sourceSlot; }
	void setSourceSlot(Slot *sourceSlot) { m_sourceSlot = sourceSlot; }

	NodeWidget * parentNode() const { return m_parentNode; }
	void setParentNode(NodeWidget *parentNode) { m_parentNode = parentNode; }

	bool isInput() const { return m_isInput; }
	void setIsInput(bool isInput) { m_isInput = isInput; }

	SlotGraphicsItem *graphicItem() const { return m_graphicItem; }
	void setGraphicItem(SlotGraphicsItem * item) { m_graphicItem = item; }

private:
	int m_maxInputs = 1;
	int m_maxOutputs = -1;
	Slot *m_sourceSlot;
	NodeWidget *m_parentNode;
	bool m_isInput;
	SlotGraphicsItem *m_graphicItem;
};

#endif // H_SLOT
