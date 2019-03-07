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
	// TODO [Elie] Why do we allow to build link with no source/dest?
	Link()
		: m_origin(nullptr), m_destination(nullptr)
	{}
	Link(OutputSlot *origin, InputSlot *destination);
	Link(const Link &) = delete; // signal emitters must not be copied
	Link & operator=(Link &) = delete;

	~Link();

	OutputSlot* origin() const { return m_origin; }
	InputSlot* destination() const { return m_destination; }

private:
	// TODO [Elie] What? Why is this still here?
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
