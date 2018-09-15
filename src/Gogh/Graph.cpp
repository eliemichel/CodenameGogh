#include "Graph.h"
#include "Node.h"
#include "Link.h"
#include "Slot.h"

Graph::~Graph() {
	while (!m_links.empty()) {
		Link *link = *m_links.begin();
		m_links.erase(m_links.begin());
		delete link;
	}

	while (!m_nodes.empty()) {
		Node *node = *m_nodes.begin();
		m_nodes.erase(m_nodes.begin());
		delete node;
	}
}

void Graph::addNode(Node *node) {
	node->destroyed.connect([this, node]() { m_nodes.erase(node); });
	m_nodes.insert(node);
	nodeAdded.fire(node);
}

void Graph::addLink(Link *link) {
	link->destroyed.connect([this, link]() { m_links.erase(link); });
	m_links.insert(link);
	linkAdded.fire(link);
}
