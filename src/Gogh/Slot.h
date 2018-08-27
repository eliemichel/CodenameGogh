#ifndef H_SLOT
#define H_SLOT

class Node;
class InputSlot;
class OutputSlot;

// TODO: add signals
class AbstractSlot {
public:
	AbstractSlot(Node *parentNode)
		: m_parentNode(parentNode)
	{}

	Node * parentNode() const { return m_parentNode; }

private:
	Node * m_parentNode;
};

class InputSlot : public AbstractSlot {
public:
	InputSlot(Node *parentNode) : AbstractSlot(parentNode) {}

	bool isConnected() const { return m_origin != nullptr; }

	OutputSlot *origin() const { return m_origin; }
	void connectTo(OutputSlot *origin) { m_origin = origin; }

	Node * originNode() const;

private:
	OutputSlot * m_origin;
};

class OutputSlot : public AbstractSlot {
public:
	OutputSlot(Node *parentNode) : AbstractSlot(parentNode) {}

	bool isConnected() const { return m_destination != nullptr; }

	InputSlot *destination() const { return m_destination; }
	void connectTo(InputSlot *destination) { m_destination = destination; }

	Node * destinationNode() const;

private:
	InputSlot * m_destination;
};

#endif // H_SLOT
