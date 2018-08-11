#include "MixNode.h"
#include "ui_MixNode.h"

#include "Logger.h"

#include <sstream>
#include <string>

MixNode::MixNode(QWidget *parent)
	: NodeWidget(parent)
	, ui(new Ui::MixNode)
	, m_node_name("mix")
{
	ui->setupUi(this);

	// Add slots
	newInputSlot();
	newInputSlot();
	newOutputSlot();
}

MixNode::~MixNode()
{
}

bool MixNode::buildRenderCommand(int outputIndex, RenderCommand & cmd) const
{
	if (outputIndex != 0) {
		return false;
	}

	if (!parentBuildRenderCommand(0, cmd))
	{
		return false;
	}

	//cmd.cmd.push_back("-c:v");
	//cmd.cmd.push_back(ui->codecBox->currentText().toStdString());
	return true;
}

// Data model

int MixNode::parmCount() const
{
	return 1;
}

QString MixNode::parmName(int parm) const
{
	if (parm >= 0 && parm < parmCount())
	{
		return "streamName" + QString(parm);
	}
	else
	{
		return QString();
	}
}

QVariant MixNode::parmEval(int parm) const
{
	if (parm >= 0 && parm < parmCount())
	{
		return ui->streamLineEdit1->text();
	}
	else
	{
		return QVariant();
	}
}

void MixNode::setParm(int parm, QVariant value)
{
	if (parm >= 0 && parm < parmCount())
	{
		ui->streamLineEdit1->setText(value.toString());
	}
}

void MixNode::slotConnectEvent(SlotEvent *event)
{
	if (event->isInputSlot() && event->slotIndex() == 0)
	{
		newInputSlot();
	}
}
