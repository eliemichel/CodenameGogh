#include "NodeWidget.h"
#include "EnvModel.h"
#include "Logger.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


NodeWidget::NodeWidget(QWidget *parent)
	: QWidget(parent)
	, m_envModel(nullptr)
{
	m_node_name = "Default";
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
	stringlist pattern;
 	buildRenderCommand(slot, cmd, pattern);
}
bool NodeWidget::buildRenderCommand(const Slot *slot, RenderCommand  & cmd, stringlist & pattern) const
{
	int i = outputSlotIndex(slot);
	return i < 0 ? false : buildRenderCommand(i, cmd, pattern);
}

bool NodeWidget::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	stringlist pattern;
	parentBuildRenderCommand(inputIndex, cmd, pattern);
}
bool NodeWidget::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd, stringlist & pattern) const
{
	if (inputIndex >= inputSlots().size())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const Slot *sourceSlot = inputSlots()[inputIndex]->sourceSlot();
	if (!sourceSlot)
	{
		ERR_LOG << "Input " << inputIndex << " is not connected, unable to render";
		return false;
	}
	const NodeWidget *parentNode = sourceSlot->parentNode();
	if (!parentNode)
	{
		ERR_LOG << "Input " << inputIndex << " is not connected to an orphan slot";
		return false;
	}

	return parentNode->buildRenderCommand(sourceSlot, cmd, pattern);
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
			SlotEvent event(i, isInput);
			slotConnectEvent(&event);
		}
		++i;
	}
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

bool NodeWidget::isPatterned(stringlist &pattern) const
{
	for (auto const& p : pattern)
	{
		if(p == m_node_name)
		{
			return true;
		}
	}
	return false;
}
