#ifndef H_SCALENODE
#define H_SCALENODE

#include "NodeWidget.h"

namespace Ui {
	class ScaleNode;
}

class ScaleNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit ScaleNode(QWidget *parent = 0);
	~ScaleNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

private:
	Ui::ScaleNode *ui;
};

#endif // H_SCALENODE
