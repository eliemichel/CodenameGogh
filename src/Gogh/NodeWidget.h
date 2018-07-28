#ifndef H_NODEWIDGET
#define H_NODEWIDGET

#include <QWidget>
#include <vector>

class LinkGraphicsItem;

enum GTest {
	G_TEST_LEFTNODE,
	G_TEST_RIGHTNODE,
};

/**
 * Typically a slot has a maxInputs or a maxOutputs set to zero, because it is respectively an output or an input node.
 * The exception is for routing nodes.
 * The slot references a widget to get its position, so that it is always correct even when complex layout cases occur.
 */
class Slot
{
public:
	const std::vector<LinkGraphicsItem*> inputLinks() const { return m_inputLinks; }
	void addInputLink(LinkGraphicsItem *link) { m_inputLinks.push_back(link); }

	const std::vector<LinkGraphicsItem*> outputLinks() const { return m_outputLinks; }
	void addOutputLink(LinkGraphicsItem *link) { m_outputLinks.push_back(link); }

	QPointF pos() const { return m_widget ? m_widget->pos() + m_widget->rect().center() : QPointF(); }
	void setWidget(QWidget *widget) { m_widget = widget; }

private:
	std::vector<LinkGraphicsItem*> m_inputLinks;
	std::vector<LinkGraphicsItem*> m_outputLinks;
	QWidget *m_widget = nullptr;
	int m_maxInputs = -1;
	int m_maxOutputs = -1;
};

class NodeWidget : public QWidget
{
	Q_OBJECT;

public:
	explicit NodeWidget(GTest test, QWidget *parent = 0);
	~NodeWidget();

	const std::vector<Slot*> allSlots() const { return m_slots; }
	Slot* newSlot() { m_slots.push_back(new Slot()); return m_slots.back(); }

private:
	std::vector<Slot*> m_slots;
};

#endif // H_NODEWIDGET