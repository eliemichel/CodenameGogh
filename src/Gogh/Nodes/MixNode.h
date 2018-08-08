#ifndef H_MIXNODE
#define H_MIXNODE

#include "NodeWidget.h"

namespace Ui {
	class MixNode;
}

class MixNode : public NodeWidget
{
	Q_OBJECT

public:
	explicit MixNode(QWidget *parent = 0);
	~MixNode();

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	QVariant parmEval(int parm) const override;
	void setParm(int parm, QVariant value) override;

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

protected:
	void slotConnectEvent(SlotEvent *event) override;

private:
	Ui::MixNode *ui;
};

#endif // H_MIXNODE
