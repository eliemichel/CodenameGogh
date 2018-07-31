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

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	QVariant parmEval(int parm) const override;
	void setParm(int parm, QVariant value) override;

private:
	Ui::ScaleNode *ui;
};

#endif // H_SCALENODE
