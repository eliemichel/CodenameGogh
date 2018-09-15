#include "Graph.h"
#include "Node.h"
#include "Link.h"
#include "Slot.h"

Graph::~Graph() {
	for (auto it = m_links.begin(); it != m_links.end();) {
		Link *link = *it;
		it = m_links.erase(it);
		delete link;
	}

	for (auto it = m_nodes.begin(); it != m_nodes.end();) {
		Node *node = *it;
		it = m_nodes.erase(it);
		delete node;
	}
}

void Graph::addNode(Node *node) {
	node->destroyed.connect([this, node]() { m_nodes.erase(node); });
	m_nodes.insert(node);
}

void Graph::addLink(Link *link) {
	link->destroyed.connect([this, link]() { m_links.erase(link); });
	m_links.insert(link);
}
