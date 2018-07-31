#ifndef H_OUTPUTNODE
#define H_OUTPUTNODE

#include "NodeWidget.h"

namespace Ui {
	class OutputNode;
}

class OutputNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit OutputNode(QWidget *parent = 0);
	~OutputNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

	void read(QDataStream & stream) override;
	void write(QDataStream & stream) const override;

private slots:
	void render();

private:
	Ui::OutputNode *ui;
};

#endif // H_OUTPUTNODE
