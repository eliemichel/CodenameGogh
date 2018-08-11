#include "TestNode.h"
#include "ui_TestNode.h"

#include "Logger.h"

TestNode::TestNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::TestNode)
{
	ui->setupUi(this);

	connect(ui->renderButton, &QPushButton::clicked, this, &TestNode::render);

	// Add slots
	newInputSlot();
	newInputSlot();
}

TestNode::~TestNode()
{
}

void TestNode::render()
{
	DEBUG_LOG << "render";
}
