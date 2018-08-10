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
	, m_envModel(nullptr)
{
}

NodeWidget::~NodeWidget()
{
	for (Slot *s : m_inputSlots)
	{
		delete s;
	}
	m_inputSlots.clear();
	for (Slot *s : m_outputSlots)
	{
		delete s;
	}
	m_outputSlots.clear();
}

Slot* NodeWidget::newInputSlot()
{
	Slot *s = new Slot();
	s->setMaxInputs(1);
	s->setMaxOutputs(0);
	s->setParentNode(this);
	s->setIsInput(true);
	m_inputSlots.push_back(s);
	return s;
}

Slot* NodeWidget::newOutputSlot()
{
	Slot *s = new Slot();
	s->setMaxInputs(1);
	s->setMaxOutputs(-1);
	s->setParentNode(this);
	s->setIsInput(false);
	m_outputSlots.push_back(s);
	return s;
}

int NodeWidget::inputSlotIndex(const Slot *slot) const
{
	const std::vector<Slot*> & inputs = inputSlots();
	for (int i = 0; i < inputs.size(); ++i)
	{
		if (inputs[i] == slot)
		{
			return i;
		}
	}
	WARN_LOG << "Invalid slot pointer provided to NodeWidget::outputSlotIndex";
	// TODO: assert(false)
	return -1;
}

const NodeGraphModel * NodeWidget::graphModel() const
{
	return modelIndex().isValid() ? static_cast<const NodeGraphModel*>(modelIndex().model()) : nullptr;
}

int NodeWidget::outputSlotIndex(const Slot *slot) const
{
	const std::vector<Slot*> & outputs = outputSlots();
	for (int i = 0; i < outputs.size(); ++i)
	{
		if (outputs[i] == slot)
		{
			return i;
		}
	}
	WARN_LOG << "Invalid slot pointer provided to NodeWidget::outputSlotIndex";
	// TODO: assert(false)
	return -1;
}

bool NodeWidget::buildRenderCommand(const Slot *slot, RenderCommand  & cmd) const
{
	int i = outputSlotIndex(slot);
	return i < 0 ? false : buildRenderCommand(i, cmd);
}

bool NodeWidget::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	if (!graphModel())
	{
		ERR_LOG << "node has no model";
	}

	if (inputIndex >= inputSlots().size())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const SlotIndex & origin = graphModel()->sourceSlot(modelIndex().row(), inputIndex);
	if (!origin.isConnected())
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

void NodeWidget::fireSlotConnectEvent(Slot *slot, bool isInput)
{
	int i = 0;
	for (Slot * s : isInput ? inputSlots() : outputSlots())
	{
		if (s == slot)
		{
			fireSlotConnectEvent(i, isInput);
		}
		++i;
	}
}

void NodeWidget::fireSlotConnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotConnectEvent(&event);
}

void NodeWidget::fireSlotDisconnectEvent(Slot *slot, bool isInput)
{
	int i = 0;
	for (Slot * s : isInput ? inputSlots() : outputSlots())
	{
		if (s == slot)
		{
			fireSlotDisconnectEvent(i, isInput);
		}
		++i;
	}
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
