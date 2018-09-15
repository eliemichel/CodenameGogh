#ifndef H_GRAPH
#define H_GRAPH

#include "Signal.h"
#include <set>

class Node;
class Link;
class InputSlot;
class OutputSlot;

class Graph {
public:
	~Graph();

	void addNode(Node *node);
	void addLink(Link *link);

	const std::set<Node*> & nodes() const { return m_nodes; }
	const std::set<Link*> & links() const { return m_links; }

public: // signals
	Signal<Node*> nodeAdded;
	Signal<Link*> linkAdded;

private:
	std::set<Node*> m_nodes;
	std::set<Link*> m_links;
};

#endif // H_GRAPH
