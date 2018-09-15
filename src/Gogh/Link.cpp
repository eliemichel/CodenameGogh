#include "Link.h"
#include "Slot.h"

Link::Link(OutputSlot *origin, InputSlot *destination)
	: m_origin(nullptr), m_destination(nullptr) 
{
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
	destroyed.fire();
}

void Link::setOrigin(OutputSlot *origin) {
	if (origin && origin != m_origin) {
		if (m_origin) {
			m_origin->removeLink(this);
		}
		m_origin = origin;
		origin->addLink(this);
	}
}

void Link::setDestination(InputSlot *destination) {
	if (destination && destination != m_destination) {
		if (m_destination) {
			m_destination->removeLink(this);
		}
		m_destination = destination;
		destination->addLink(this);
	}
}
