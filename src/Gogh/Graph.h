#ifndef H_GRAPH
#define H_GRAPH

#include <vector>

class Node;

class Graph {
public:
	void addNode(Node *node) { m_nodes.push_back(node); }
	const std::vector<Node*> & nodes() const { return m_nodes; }

private:
	std::vector<Node*> m_nodes;
};

#endif // H_GRAPH
