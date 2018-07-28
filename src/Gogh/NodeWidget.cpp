#include "NodeWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

NodeWidget::NodeWidget(GTest test, QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout *mainLayout, *layout;
	QWidget *w;
	switch (test)
	{
	case G_TEST_LEFTNODE:
		mainLayout = new QHBoxLayout();
		mainLayout->addWidget(new QLabel("Path"));
		mainLayout->addWidget(new QLineEdit("input.mp4"));
		w = new QPushButton();
		newSlot()->setWidget(w);
		mainLayout->addWidget(w);
		this->setLayout(mainLayout);
		break;

	case G_TEST_RIGHTNODE:
		mainLayout = new QVBoxLayout();
		layout = new QHBoxLayout();
		w = new QPushButton();
		newSlot()->setWidget(w);
		layout->addWidget(w);
		layout->addWidget(new QLabel("Path"));
		layout->addWidget(new QLineEdit("output1.mov"));
		mainLayout->addLayout(layout);
		layout = new QHBoxLayout();
		w = new QPushButton();
		newSlot()->setWidget(w);
		layout->addWidget(w);
		layout->addWidget(new QLabel("Path"));
		layout->addWidget(new QLineEdit("output2.mov"));
		mainLayout->addLayout(layout);
		this->setLayout(mainLayout);
		break;
	}
}

NodeWidget::~NodeWidget()
{
	for (Slot *s : m_slots)
	{
		delete s;
	}
	m_slots.clear();
}
