#include "Slot.h"
#include "Link.h"

#include <cassert>

AbstractSlot::~AbstractSlot() {
	deleteLinks();
}

void AbstractSlot::addLink(Link *link) {
	m_links.insert(link);
}

void AbstractSlot::removeLink(Link *link) {
	for (auto it = m_links.begin(); it != m_links.end(); ) {
		if (*it == link) {
			it = m_links.erase(it);
		} else {
			++it;
		}
	}
}

void AbstractSlot::deleteLinks() {
	while (!m_links.empty()) {
		Link *link = *m_links.begin();
		m_links.erase(m_links.begin());
		delete link;
	}
}

void InputSlot::addLink(Link *link) {
	if (!link) {
		return;
	}

	// Remove previous link
	assert(links().empty() || links().size() == 1);
	deleteLinks();
	
	link->setDestination(this);
	AbstractSlot::addLink(link);
}

void OutputSlot::addLink(Link *link) {
	if (!link) {
		return;
	}

	link->setOrigin(this);
	AbstractSlot::addLink(link);
}
