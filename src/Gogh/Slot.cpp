#include "Slot.h"

Node * InputSlot::originNode() const {
	return isConnected() ? m_origin->parentNode() : nullptr;
}

Node * OutputSlot::destinationNode() const {
	return isConnected() ? m_destination->parentNode() : nullptr;
}
