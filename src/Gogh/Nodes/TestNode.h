#ifndef H_TESTNODE
#define H_TESTNODE

#include "NodeWidget.h"

namespace Ui {
	class TestNode;
}

class TestNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit TestNode(QWidget *parent = 0);
	~TestNode();

private slots:
	void render();

private:
	Ui::TestNode *ui;
};

#endif // H_TESTNODE
