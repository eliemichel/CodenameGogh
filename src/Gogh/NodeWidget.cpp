#include "NodeWidget.h"
#include "EnvModel.h"
#include "Logger.h"
#include "NodeGraphModel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


NodeWidget::NodeWidget(QWidget *parent)
	: QWidget(parent)
	, m_inputSlotsCount(0)
	, m_outputSlotsCount(0)
	, m_envModel(nullptr)
	, m_node_name("Default")
{

}

void NodeWidget::newInputSlot()
{
	m_inputSlotsCount++;

	if (graphModel())
	{
		graphModel()->addInputSlot(modelIndex());
	}
}

void NodeWidget::newOutputSlot()
{
	m_outputSlotsCount++;

	if (graphModel())
	{
		graphModel()->addOutputSlot(modelIndex());
	}
}

bool NodeWidget::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	if (!graphModel() || !modelIndex().isValid())
	{
		ERR_LOG << "node has no model";
	}

	if (inputIndex >= inputSlotsCount())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const SlotIndex & origin = graphModel()->originSlot(SlotIndex(modelIndex().row(), inputIndex));
	if (!origin.isValid())
	{
		ERR_LOG << "Input " << inputIndex << " is not connected, unable to render";
		return false;
	}

	const NodeWidget *parentNode = graphModel()->nodeData(origin.node);
	if (!parentNode)
	{
		ERR_LOG << "Input " << inputIndex << " is connected to an orphan slot";
		return false;
	}

	return parentNode->buildRenderCommand(origin.slot, cmd);
}

void NodeWidget::read(QDataStream & stream)
{
	int n;
	QVariant v;
	stream >> n;
	for (int i = 0; i < std::min(n, parmCount()); ++i)
	{
		stream >> v;
		setParm(i, v);
	}
}

void NodeWidget::write(QDataStream & stream) const
{
	int n = parmCount();
	stream << n;
	for (int i = 0; i < n; ++i)
	{
		stream << parmEval(i);
	}
}

void NodeWidget::fireSlotConnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotConnectEvent(&event);
}

void NodeWidget::fireSlotDisconnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotDisconnectEvent(&event);
}

QString NodeWidget::parmFullEval(int parm) const
{
	QString value = parmEval(parm).toString();
	if (EnvModel *env = envModel()) {
		for (auto it = env->env().cbegin(); it != env->env().cend(); ++it)
		{
			value = value.replace(QString::fromStdString("$" + it->first), QString::fromStdString(it->second));
		}
	}
	return value;
}
