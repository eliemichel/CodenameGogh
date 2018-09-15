#ifndef H_LINK
#define H_LINK

#include "Signal.h"

class InputSlot;
class OutputSlot;

class Link
{
public:
	Link()
		: m_origin(nullptr), m_destination(nullptr)
	{}

	Link(OutputSlot *origin, InputSlot *destination);

	~Link();

	OutputSlot* origin() const { return m_origin; }
	void setOrigin(OutputSlot *origin);

	InputSlot* destination() const { return m_destination; }
	void setDestination(InputSlot *destination);

public: // signals
	/// Signal emitted just after destructing the object
	Signal<> destroyed;

private:
	OutputSlot *m_origin;
	InputSlot *m_destination;
};

#endif // H_LINK
