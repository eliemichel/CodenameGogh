#include "Slot.h"

#include "Logger.h"

void Slot::setMaxInputs(int maxInputs) {
	if (maxInputs != 0 && maxInputs != 1) {
		WARN_LOG << "Maximum number of inputs on a slot can only be 0 or 1 (" << maxInputs << " was provided)";
		return;
	}
	m_maxInputs = maxInputs;
}
