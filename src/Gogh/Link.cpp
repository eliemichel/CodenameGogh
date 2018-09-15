#include "Link.h"
#include "Slot.h"

Link::Link(OutputSlot *origin, InputSlot *destination) {
	setOrigin(origin);
	setDestination(destination);
}

Link::~Link() {
	if (m_origin) {
		m_origin->removeLink(this);
	}
	if (m_destination) {
		m_destination->removeLink(this);
	}
}

void Link::setOrigin(OutputSlot *origin) {
	if (origin && origin != m_origin) {
		m_origin = origin;
		origin->addLink(this);
	}
}

void Link::setDestination(InputSlot *destination) {
	if (destination && destination != m_destination) {
		m_destination = destination;
		destination->addLink(this);
	}
}
