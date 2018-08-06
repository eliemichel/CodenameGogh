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

bool NodeWidget::buildRenderCommand(const Slot *slot, RenderCommand  & cmd) const
{
	const std::vector<Slot*> & outputs = outputSlots();
	for (int i = 0; i < outputs.size(); ++i)
	{
		if (outputs[i] == slot)
		{
			return buildRenderCommand(i, cmd);
		}
	}
	WARN_LOG << "Invalid slot pointer provided to NodeWidget::buildRenderCommand";
	return false;
}

bool NodeWidget::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
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

	return parentNode->buildRenderCommand(sourceSlot, cmd);
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
