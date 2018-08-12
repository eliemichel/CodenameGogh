#include "Node.h"
#include "EnvModel.h"
#include "NodeGraphModel.h"
#include "DefaultNodeEditor.h"
#include "Logger.h"

Node::Node(QObject *parent)
	: QObject(parent)
	, m_envModel(nullptr)
	, m_graphModel(nullptr)
{
}

QWidget *Node::createEditor(QWidget *parent)
{
	return new DefaultNodeEditor(this, parent);
}

bool Node::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	if (!graphModel())
	{
		ERR_LOG << "node has no model";
	}

	if (inputIndex >= inputLinks.size())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const SlotIndex & origin = inputLinks[inputIndex];
	if (!origin.isValid())
	{
		ERR_LOG << "Input " << inputIndex << " is not connected, unable to render";
		return false;
	}

	const Node *parentNode = graphModel()->node(origin.node);
	if (!parentNode)
	{
		ERR_LOG << "Input " << inputIndex << " is connected to an orphan slot";
		return false;
	}

	return parentNode->buildRenderCommand(origin.slot, cmd);
}

void Node::read(QDataStream & stream)
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

void Node::write(QDataStream & stream) const
{
	int n = parmCount();
	stream << n;
	for (int i = 0; i < n; ++i)
	{
		stream << parmRawValue(i);
	}
}

void Node::fireSlotConnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotConnectEvent(&event);
}

void Node::fireSlotDisconnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotDisconnectEvent(&event);
}

QString Node::parmEvalAsString(int parm) const
{
	QString value;
	switch (parmType(parm))
	{
	case EnumType:
	{
		int menu = parmRawValue(parm).toInt();
		value = parmMenuValue(parm, menu).toString();
		break;
	}
	default:
		value = parmRawValue(parm).toString();
	}

	if (EnvModel *env = envModel()) {
		for (auto it = env->env().cbegin(); it != env->env().cend(); ++it)
		{
			value = value.replace(QString::fromStdString("$" + it->first), QString::fromStdString(it->second));
		}
	}
	return value;
}

int Node::parmEvalAsInt(int parm) const
{
	QString value;
	switch (parmType(parm))
	{
	case EnumType:
	{
		int menu = parmRawValue(parm).toInt();
		return parmMenuValue(parm, menu).toInt();
		break;
	}
	default:
		return parmRawValue(parm).toInt();
	}
}

void Node::newInputSlot()
{
	inputLinks.resize(inputLinks.size() + 1);
	if (graphModel())
	{
		graphModel()->broadcastNodeChange(nodeIndex());
	}
}

void Node::newOutputSlot()
{
	outputLinks.resize(outputLinks.size() + 1);
	if (graphModel())
	{
		graphModel()->broadcastNodeChange(nodeIndex());
	}
}

void Node::removeOutputSlots()
{
	if (!graphModel())
	{
		return;
	}

	// Remove links
	for (std::set<SlotIndex> & destinationSet : outputLinks)
	{
		while (!destinationSet.empty())
		{
			graphModel()->removeLink(*destinationSet.begin());
		}
	}

	// Remove slots
	// TODO: remove graphicsslots
	outputLinks.clear();

	graphModel()->broadcastNodeChange(nodeIndex());
}
