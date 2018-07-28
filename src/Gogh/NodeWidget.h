#ifndef H_NODEWIDGET
#define H_NODEWIDGET

#include "Slot.h"
#include <QWidget>
#include <vector>
#include <string>

class LinkGraphicsItem;

struct RenderCommand {
	std::string cmd;
};

enum GTest {
	G_TEST_LEFTNODE,
	G_TEST_RIGHTNODE,
};

class NodeWidget : public QWidget
{
	Q_OBJECT;

public:
	explicit NodeWidget(QWidget *parent = 0);
	explicit NodeWidget(GTest test, QWidget *parent = 0);
	~NodeWidget();

	const std::vector<Slot*> inputSlots() const { return m_inputSlots; }
	Slot* newInputSlot();

	const std::vector<Slot*> outputSlots() const { return m_outputSlots; }
	Slot* newOutputSlot();

	bool buildRenderCommand(const Slot *slot, RenderCommand  & cmd) const;
	virtual bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const { return true; }

private:
	std::vector<Slot*> m_inputSlots;
	std::vector<Slot*> m_outputSlots;
};

#endif // H_NODEWIDGET