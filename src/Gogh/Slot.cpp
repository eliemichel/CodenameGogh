#include "Slot.h"
#include "Link.h"

#include <cassert>

void AbstractSlot::removeLink(Link *link) {
	for (auto it = m_links.begin(); it != m_links.end(); ) {
		if (*it == link) {
			it = m_links.erase(it);
		} else {
			++it;
		}
	}
}

void AbstractSlot::addLink(Link *link) {
	m_links.insert(link);
}

void InputSlot::addLink(Link *link) {
	if (!link) {
		return;
	}

	// Remove previous link
	assert(links().empty() || links().size() == 1);
	for (Link * l : links()) {
		delete l;
	}
	links().clear();
	
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
