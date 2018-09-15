#ifndef H_SLOT
#define H_SLOT

#include <set>

class Node;
class InputSlot;
class OutputSlot;
class Link;

// TODO: add signals
class AbstractSlot {
public:
	AbstractSlot(Node *parentNode)
		: m_parentNode(parentNode)
	{}
	AbstractSlot(const AbstractSlot &) = delete; // signal emitters must not be copied

	Node * parentNode() const { return m_parentNode; }

	void removeLink(Link *link);
	virtual void addLink(Link *link);

protected:
	std::set<Link*> & links() { return m_links; }
	const std::set<Link*> & links() const { return m_links; }

private:
	Node * m_parentNode;
	std::set<Link*> m_links;
};

class InputSlot : public AbstractSlot {
public:
	InputSlot(Node *parentNode) : AbstractSlot(parentNode) {}

	void addLink(Link *link) override;
};

class OutputSlot : public AbstractSlot {
public:
	OutputSlot(Node *parentNode) : AbstractSlot(parentNode) {}

	void addLink(Link *link) override;
};

#endif // H_SLOT
