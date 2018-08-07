#ifndef H_LINK
#define H_LINK

class Slot;

class Link
{
public:
	Link()
		: m_origin(nullptr), m_destination(nullptr)
	{}

	Link(Slot *origin, Slot *destination)
		: m_origin(origin), m_destination(destination)
	{}

	Slot* origin() const { return m_origin; }
	void setOrigin(Slot *origin) { m_origin = origin; }

	Slot* destination() const { return m_destination; }
	void setDestination(Slot *destination) { m_destination = destination; }

private:
	Slot *m_origin;
	Slot *m_destination;
};
