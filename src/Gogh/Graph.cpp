#include "Graph.h"
#include "Node.h"
#include "Link.h"
#include "Slot.h"

Graph::~Graph() {
	while (!m_links.empty()) {
		delete m_links.back();
		m_links.pop_back();
	}

	while (!m_nodes.empty()) {
		delete m_nodes.back();
		m_nodes.pop_back();
	}
}

void Graph::addLink(OutputSlot *origin, InputSlot *destination) {
	destination->Node();
}
