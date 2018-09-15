#ifndef H_SLOT
#define H_SLOT

#include <set>

class Node;
class InputSlot;
class OutputSlot;
class Link;

class AbstractSlot {
public:
	AbstractSlot(Node *parentNode)
		: m_parentNode(parentNode)
	{}
	~AbstractSlot();

	Node * parentNode() const { return m_parentNode; }

	virtual void addLink(Link *link);
	/// This "forgets" the link( remove the reference to it) but does not delete it
	void removeLink(Link *link);
	/// Delete the link object and remove them
	void deleteLinks();

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
