#ifndef H_GRAPH
#define H_GRAPH

#include <vector>

class Node;
class Link;
class InputSlot;
class OutputSlot;

class Graph {
public:
	~Graph();

	void addNode(Node *node) { m_nodes.push_back(node); }
	void addLink(OutputSlot *origin, InputSlot *destination);

	const std::vector<Node*> & nodes() const { return m_nodes; }
	const std::vector<Link*> & links() const { return m_links; }

private:
	std::vector<Node*> m_nodes;
	std::vector<Link*> m_links;
};

#endif // H_GRAPH
