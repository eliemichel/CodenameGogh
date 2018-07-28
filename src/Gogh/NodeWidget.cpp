#include "NodeWidget.h"
#include "Logger.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


NodeWidget::NodeWidget(QWidget *parent)
	: QWidget(parent)
{
}

NodeWidget::NodeWidget(GTest test, QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout *mainLayout, *layout;
	switch (test)
	{
	case G_TEST_LEFTNODE:
		mainLayout = new QHBoxLayout();
		mainLayout->addWidget(new QLabel("Path"));
		mainLayout->addWidget(new QLineEdit("input.mp4"));
		mainLayout->addWidget(new QWidget());
		this->setLayout(mainLayout);

		newOutputSlot();
		break;

	case G_TEST_RIGHTNODE:
		mainLayout = new QVBoxLayout();
		layout = new QHBoxLayout();
		layout->addWidget(new QWidget());
		layout->addWidget(new QLabel("Path"));
		layout->addWidget(new QLineEdit("output1.mov"));
		mainLayout->addLayout(layout);
		layout = new QHBoxLayout();
		layout->addWidget(new QWidget());
		layout->addWidget(new QLabel("Path"));
		layout->addWidget(new QLineEdit("output2.mov"));
		mainLayout->addLayout(layout);
		layout = new QHBoxLayout();
		layout->addWidget(new QPushButton("Render"));
		mainLayout->addLayout(layout);
		this->setLayout(mainLayout);

		newInputSlot();
		newInputSlot();
		break;
	}
}

NodeWidget::~NodeWidget()
{
	for (Slot *s : m_inputSlots)
	{
		delete s;
	}
	m_inputSlots.clear();
	for (Slot *s : m_outputSlots)
	{
		delete s;
	}
	m_outputSlots.clear();
}

Slot* NodeWidget::newInputSlot()
{
	Slot *s = new Slot();
	s->setMaxInputs(1);
	s->setMaxOutputs(0);
	s->setParentNode(this);
	m_inputSlots.push_back(s);
	return s;
}

Slot* NodeWidget::newOutputSlot()
{
	Slot *s = new Slot();
	s->setMaxInputs(1);
	s->setMaxOutputs(-1);
	s->setParentNode(this);
	m_outputSlots.push_back(s);
	return s;
}

bool NodeWidget::buildRenderCommand(const Slot *slot, RenderCommand  & cmd) const
{
	const std::vector<Slot*> & outputs = outputSlots();
	for (int i = 0; i < outputs.size(); ++i)
	{
		if (outputs[i] == slot)
		{
			return buildRenderCommand(i, cmd);
		}
	}
	WARN_LOG << "Invalid slot pointer provided to NodeWidget::buildRenderCommand";
	return false;
}
