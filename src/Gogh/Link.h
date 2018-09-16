#ifndef H_LINK
#define H_LINK

#include "Signal.h"

class InputSlot;
class OutputSlot;

/**
 * NB: A link has immutable origin/destination
 */
class Link
{
public:
	Link()
		: m_origin(nullptr), m_destination(nullptr)
	{}
	Link(OutputSlot *origin, InputSlot *destination);
	Link(const Link &) = delete; // signal emitters must not be copied

	~Link();

	OutputSlot* origin() const { return m_origin; }
	InputSlot* destination() const { return m_destination; }

private:
	void setOrigin(OutputSlot *origin);
	void setDestination(InputSlot *destination);

public: // signals
	/// Signal emitted just after destructing the object
	Signal<> destroyed;

private:
	OutputSlot *m_origin;
	InputSlot *m_destination;
};

#endif // H_LINK
