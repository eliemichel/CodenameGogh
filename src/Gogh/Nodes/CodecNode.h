#ifndef H_CODECNODE
#define H_CODECNODE

#include "NodeWidget.h"

namespace Ui {
	class CodecNode;
}

class CodecNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit CodecNode(QWidget *parent = 0);
	~CodecNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

private:
	Ui::CodecNode *ui;
};

#endif // H_CODECNODE
