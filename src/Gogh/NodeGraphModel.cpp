#include "NodeGraphModel.h"

#include "Logger.h"

#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"
#include "Nodes/MixNode.h"

#include <QFile>
#include <QDataStream>
#include <QModelIndex>

SlotIndex NodeGraphModel::invalidSlot = SlotIndex();
std::set<SlotIndex> NodeGraphModel::invalidSlotSet = std::set<SlotIndex>();

NodeGraphModel::NodeGraphModel()
	: QAbstractItemModel()
	, m_envModel(nullptr)
{}

NodeGraphModel::~NodeGraphModel()
{
	while (!m_nodes.empty())
	{
		NodeEntry *node = m_nodes.back();
		delete node;
		m_nodes.pop_back();
	}
}

NodeWidget * NodeGraphModel::buildNode(int type)
{
	switch (type)
	{
	case NODE_INPUT:
		return new InputNode();
	case NODE_OUTPUT:
		return new OutputNode();
	case NODE_SCALE:
		return new ScaleNode();
	case NODE_CODEC:
		return new CodecNode();
	case NODE_MIX:
		return new MixNode();
	default:
		return nullptr;
	}
}

std::string NodeGraphModel::nodeTypeToString(int type)
{
	switch (type)
	{
	case NODE_INPUT:
		return "NODE_INPUT";
	case NODE_OUTPUT:
		return "NODE_OUTPUT";
	case NODE_SCALE:
		return "NODE_SCALE";
	case NODE_CODEC:
		return "NODE_CODEC";
	case NODE_MIX:
		return "NODE_MIX";
	default:
		return nullptr;
	}
}

bool NodeGraphModel::isRoot(QModelIndex index)
{
	return !index.isValid();
}

bool NodeGraphModel::canAddLink(int originNodeIndex, int originSlot, int destinationNodeIndex, int destinationSlot)
{
	if (originNodeIndex < 0 || originNodeIndex >= m_nodes.size() || destinationNodeIndex < 0 || destinationNodeIndex >= m_nodes.size())
	{
		return false;
	}
	const NodeEntry *originNode = m_nodes[originNodeIndex];
	const NodeEntry *destinationNode = m_nodes[destinationNodeIndex];
	if (originSlot < 0 || originSlot >= originNode->outputLinks.size() || destinationSlot < 0 || destinationSlot >= destinationNode->inputLinks.size())
	{
		return false;
	}
	// TODO: check for graph loops
	return true;
}

bool NodeGraphModel::addLink(int originNodeIndex, int originSlot, int destinationNodeIndex, int destinationSlot)
{
	if (!canAddLink(originNodeIndex, originSlot, destinationNodeIndex, destinationSlot))
	{
		return false;
	}
	NodeEntry *originNode = m_nodes[originNodeIndex];
	NodeEntry *destinationNode = m_nodes[destinationNodeIndex];

	SlotIndex destination(destinationNodeIndex, destinationSlot);
	SlotIndex origin(originNodeIndex, originSlot);

	// unplug previous link
	SlotIndex oldOrigin = destinationNode->inputLinks[destinationSlot];
	bool wasConnected = oldOrigin.isValid();
	if (wasConnected)
	{
		NodeEntry *oldOriginEntry = m_nodes[oldOrigin.node];
		oldOriginEntry->outputLinks[oldOrigin.slot].erase(destination);
	}

	// plug new link
	originNode->outputLinks[originSlot].insert(destination);
	destinationNode->inputLinks[destinationSlot] = origin;

	// signal change to views
	const QModelIndex & oldSourceIndex = index(oldOrigin.node, 0);
	if (wasConnected)
	{
		emit dataChanged(oldSourceIndex, oldSourceIndex);
	}
	const QModelIndex & newSourceIndex = index(originNodeIndex, 0);
	const QModelIndex & destinationIndex = index(destinationNodeIndex, 0);
	emit dataChanged(newSourceIndex, newSourceIndex);
	emit dataChanged(destinationIndex, destinationIndex);

	// signal change to nodes
	if (wasConnected)
	{
		NodeEntry *oldOriginEntry = m_nodes[oldOrigin.node];
		if (oldOriginEntry->node)
		{
			oldOriginEntry->node->fireSlotDisconnectEvent(oldOrigin.slot, false /* isInput */);
		}
	}
	if (originNode->node)
	{
		originNode->node->fireSlotConnectEvent(origin.slot, false /* isInput */);
	}
	if (destinationNode->node)
	{
		destinationNode->node->fireSlotConnectEvent(destination.slot, true /* isInput */);
	}

	return true;
}

bool NodeGraphModel::removeLink(int destinationNodeIndex, int destinationSlot)
{
	if (destinationNodeIndex < 0 || destinationNodeIndex >= m_nodes.size())
	{
		DEBUG_LOG << "invalid destination node";
		return false;
	}

	NodeEntry *destinationNode = m_nodes[destinationNodeIndex];
	if (destinationSlot < 0 || destinationSlot >= destinationNode->inputLinks.size())
	{
		DEBUG_LOG << "invalid destination slot: #" << destinationSlot << " (in node #" << destinationNodeIndex << ")";
		return false;
	}

	SlotIndex & origin = destinationNode->inputLinks[destinationSlot];
	if (!origin.isValid())
	{
		DEBUG_LOG << "slot had no connection";
		return false;
	}

	// remove link
	SlotIndex destination(destinationNodeIndex, destinationSlot);
	NodeEntry *originNode = m_nodes[origin.node];
	originNode->outputLinks[origin.slot].erase(destination);
	origin.node = -1;

	// signal views
	const QModelIndex & originIndex = index(origin.node, 0);
	const QModelIndex & destinationIndex = index(destinationNodeIndex, 0);
	emit dataChanged(originIndex, originIndex);
	emit dataChanged(destinationIndex, destinationIndex);

	// signal node
	if (originNode->node)
	{
		originNode->node->fireSlotDisconnectEvent(origin.slot, false /* isInput */);
	}
	if (destinationNode->node)
	{
		destinationNode->node->fireSlotDisconnectEvent(destination.slot, true /* isInput */);
	}

	return true;
}

const SlotIndex & NodeGraphModel::sourceSlot(int destinationNodeIndex, int destinationSlot) const
{
	if (destinationNodeIndex < 0 || destinationNodeIndex >= m_nodes.size())
	{
		WARN_LOG << "Invalid destination node: #" << destinationNodeIndex;
		return invalidSlot;
	}
	const NodeEntry *destinationNode = m_nodes[destinationNodeIndex];
	if (destinationSlot < 0 || destinationSlot >= destinationNode->inputLinks.size())
	{
		WARN_LOG << "Invalid destination slot: #" << destinationSlot << " (in node #" << destinationNodeIndex << ")";
		return invalidSlot;
	}
	return destinationNode->inputLinks[destinationSlot];
}

const std::set<SlotIndex> & NodeGraphModel::destinationSlots(int sourceNodeIndex, int sourceSlot) const
{
	if (sourceNodeIndex < 0 || sourceNodeIndex >= m_nodes.size())
	{
		WARN_LOG << "Invalid source node: #" << sourceNodeIndex;
		return invalidSlotSet;
	}
	const NodeEntry *sourceNode = m_nodes[sourceNodeIndex];
	if (sourceSlot < 0 || sourceSlot >= sourceNode->outputLinks.size())
	{
		WARN_LOG << "Invalid source slot: #" << sourceSlot << " (in node #" << sourceNodeIndex << ")";
		return invalidSlotSet;
	}
	return sourceNode->outputLinks[sourceSlot];
}

void NodeGraphModel::nodeGeometryChanged(const QModelIndex & nodeIndex)
{
	// TODO emit change on slot subindex
	if (nodeIndex.isValid())
	{
		NodeEntry *entry = m_nodes[nodeIndex.row()];
		entry->inputLinks.resize(entry->node->inputSlotsCount());
		entry->outputLinks.resize(entry->node->outputSlotsCount());
		emit dataChanged(nodeIndex, nodeIndex);
	}
}

bool NodeGraphModel::inParentBounds(const QModelIndex & index) const
{
	return index.row() >= 0 && index.row() < rowCount(index.parent()) && index.column() >= 0 && index.column() < columnCount(index.parent());
}

NodeGraphModel::IndexData * NodeGraphModel::indexData(const QModelIndex & index) const
{
	return index.isValid() ? static_cast<IndexData*>(index.internalPointer()) : nullptr;
}

QModelIndex NodeGraphModel::index(int row, int column, const QModelIndex & parent) const
{
	if (row < 0 || row >= rowCount(parent) || column < 0 || column >= columnCount(parent))
	{
		return QModelIndex();
	}

	IndexData *data = indexData(parent);
	IndexLevel level = data ? data->level : RootLevel;
	switch (level)
	{

	case RootLevel:
		return createIndex(row, column, &m_nodes[row]->nodeIndex);

	case NodeLevel:
		return createIndex(row, column, &m_nodes[parent.row()]->blockIndex);

	case BlockLevel:
		return createIndex(row, column, &m_nodes[data->parentNodeIndex]->elementIndex[parent.row()]);

	default:
		return QModelIndex();

	}
}

QModelIndex NodeGraphModel::parent(const QModelIndex & index) const
{
	if (!index.isValid() || !index.internalPointer())
	{
		return QModelIndex();
	}
	else
	{
		IndexData *data = static_cast<IndexData*>(index.internalPointer());
		NodeEntry *node = m_nodes[data->parentNodeIndex];
		switch (data->level)
		{
		case NodeLevel:
			return QModelIndex();
		case BlockLevel:
			return createIndex(data->parentNodeIndex, 0, &node->nodeIndex);
		case ElementLevel:
			return createIndex(data->parentBlockIndex, 0, &node->blockIndex);
		default:
			return QModelIndex();
		}
	}
}

int NodeGraphModel::rowCount(const QModelIndex & parent) const
{
	IndexData *data = indexData(parent);
	IndexLevel level = data ? data->level : RootLevel;
	NodeWidget *nodeWidget = data ? m_nodes[data->parentNodeIndex]->node : nullptr;
	switch (level)
	{

	case RootLevel:
		return static_cast<int>(m_nodes.size());

	case NodeLevel:
		return _BlockCount;

	case BlockLevel:
		switch (parent.row())
		{
		case ParamBlock:
			return nodeWidget->parmCount();
		case InputSlotBlock:
			return nodeWidget->inputSlotsCount();
		case OutputSlotBlock:
			return nodeWidget->outputSlotsCount();
		}

	default:
		return 0;

	}
}

int NodeGraphModel::columnCount(const QModelIndex & parent) const
{
	IndexData *data = indexData(parent);
	IndexLevel level = data ? data->level : RootLevel;
	switch (level)
	{

	case RootLevel:
		return _ColumnCount;

	case NodeLevel:
		return 1;

	case BlockLevel:
		switch (parent.row())
		{
		case ParamBlock:
			return 2; // parmName and parmEval
		case InputSlotBlock:
			return 2; // link origin node and slot
		case OutputSlotBlock:
			return 2; // link destination node and slot
		}

	default:
		return 0;

	}
}

QVariant NodeGraphModel::data(const QModelIndex & index, int role) const
{
	if (role != Qt::DisplayRole && role != Qt::ToolTipRole && role != Qt::WhatsThisRole && role != Qt::EditRole)
	{
		return QVariant();
	}

	IndexData *data = indexData(index);
	IndexLevel level = data ? data->level : RootLevel;
	NodeWidget *nodeWidget = data ? m_nodes[data->parentNodeIndex]->node : nullptr;
	switch (level)
	{

	case RootLevel:
		return QVariant();

	case NodeLevel:
	{
		NodeEntry *node = m_nodes[index.row()];
		switch (index.column())
		{
		case TypeColumn:
			if (role == Qt::EditRole)
			{
				return node->type;
			}
			else
			{
				return QString::fromStdString(nodeTypeToString(node->type));
			}
		case PosXColumn:
			return node->x;
		case PosYColumn:
			return node->y;
		case NameColumn:
			return QString::fromStdString(node->name);
		default:
			return QVariant();
		}
	}

	case BlockLevel:
		switch (index.row())
		{
		case ParamBlock:
			return "Parameters";
		case InputSlotBlock:
			return "Input Slots";
		case OutputSlotBlock:
			return "Output Slots";
		}

	case ElementLevel:
		switch (index.parent().row())
		{
		case ParamBlock:
			switch (index.column())
			{
			case 0: // name
				return nodeWidget->parmName(index.row());
			case 1: // value
				return nodeWidget->parmEval(index.row());
			default:
				return QVariant();
			}
		case InputSlotBlock:
		{
			SlotIndex sourceSlot = m_nodes[data->parentNodeIndex]->inputLinks[index.row()];
			
			if (role == Qt::EditRole)
			{
				switch (index.column())
				{
				case 0: // node
					return sourceSlot.node;
				case 1: // slot
					return sourceSlot.slot;
				default:
					return QVariant();
				}
			}
			else
			{
				if (!sourceSlot.isValid())
				{
					return index.column() == 0 ? "<not connected>" : QVariant();
				}

				switch (index.column())
				{
				case 0: // node
					return QString("Node #") + QString::number(sourceSlot.node);
				case 1: // slot
					return QString("Output Slot #") + QString::number(sourceSlot.slot);
				default:
					return QVariant();
				}
			}
		}
		case OutputSlotBlock:
			// TODO: expose all outputs to the model
			const std::set<SlotIndex> & destinations = m_nodes[data->parentNodeIndex]->outputLinks[index.row()];
			const SlotIndex & destinationSlot = destinations.begin() == destinations.end() ? SlotIndex() : *destinations.begin();

			if (role == Qt::EditRole)
			{
				switch (index.column())
				{
				case 0: // node
					return destinationSlot.node;
				case 1: // slot
					return destinationSlot.slot;
				default:
					return QVariant();
				}
			}
			else
			{
				if (!destinationSlot.isValid())
				{
					return index.column() == 0 ? "<not connected>" : QVariant();
				}

				switch (index.column())
				{
				case 0: // node
					return QString("Node #") + QString::number(destinationSlot.node);
				case 1: // slot
					return QString("Input Slot #") + QString::number(destinationSlot.slot);
				default:
					return QVariant();
				}
			}
		}

	default:
		return QVariant();
	}
}

QVariant NodeGraphModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		switch (section)
		{
		case TypeColumn:
			return "type";
		case PosXColumn:
			return "x";
		case PosYColumn:
			return "y";
		case NameColumn:
			return "name";
		default:
			return QVariant();
		}
	}
	else
	{
		return QVariant();
	}
}

bool NodeGraphModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role != Qt::EditRole)
	{
		return false;
	}

	IndexData *data = indexData(index);
	IndexLevel level = data ? data->level : RootLevel;
	switch (level)
	{

	case RootLevel:
		return false;

	case NodeLevel:
	{
		NodeEntry *node = m_nodes[index.row()];
		switch (index.column())
		{
		case PosXColumn:
			node->x = value.toFloat();
			break;
		case PosYColumn:
			node->y = value.toFloat();
			break;
		case NameColumn:
			node->name = value.toString().toStdString();
			break;
		default:
			return false;
		}
		emit dataChanged(index, index);
		return true;
	}

	case BlockLevel:
		return false;

	case ElementLevel:
	{
		NodeWidget * nodeWidget = data ? nodes()[data->parentNodeIndex]->node : nullptr;
		switch (index.parent().row())
		{
		case ParamBlock:
			switch (index.column())
			{
			case 1: // value
				nodeWidget->setParm(index.row(), value);
				emit dataChanged(index, index);
				return true;
			default:
				return false;
			}
		case InputSlotBlock:
		{
			if (!value.canConvert<int>())
			{
				return false;
			}
			int id = value.toInt();

			SlotIndex & sourceSlot = m_nodes[data->parentNodeIndex]->inputLinks[index.row()];

			switch (index.column())
			{
			case 0: // node
			{
				if (id < 0 || id >= nodes().size() || id == sourceSlot.node)
				{
					return false;
				}

				// clamp slot
				int n = static_cast<int>(m_nodes[id]->outputLinks.size());
				if (n == 0)
				{
					return false;
				}
				int slot = std::max(0, std::min(sourceSlot.slot, n - 1));

				addLink(id, slot, data->parentNodeIndex, index.row());

				return true;
			}
			case 1: // slot
			{
				if (!sourceSlot.isValid())
				{
					return false;
				}

				int n = static_cast<int>(m_nodes[sourceSlot.node]->outputLinks.size());
				if (id < 0 || id >= n)
				{
					return false;
				}

				addLink(sourceSlot.node, id, data->parentNodeIndex, index.row());

				return true;
			}
			default:
				return false;
			}
		}
		case OutputSlotBlock:
			return false;
		}
	}

	default:
		return false;
	}
}

Qt::ItemFlags NodeGraphModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags f = QAbstractItemModel::flags(index);

	IndexData *data = indexData(index);
	IndexLevel level = data ? data->level : RootLevel;
	switch (level)
	{

	case RootLevel:
		return f;

	case NodeLevel:
		switch (index.column())
		{
		case PosXColumn:
		case PosYColumn:
		case NameColumn:
			return f | Qt::ItemIsEditable;
		default:
			return f;
		}

	case BlockLevel:
		return f;

	case ElementLevel:
	{
		switch (index.parent().row())
		{
		case ParamBlock:
			switch (index.column())
			{
			case 1: // value
				return f | Qt::ItemIsEditable;
			default:
				return f;
			}
		case InputSlotBlock:
			return f | Qt::ItemIsEditable;
		case OutputSlotBlock:
			return f;
		}
	}

	default:
		return f;
	}
}

void NodeGraphModel::LoadDefaultGraph()
{
	NodeWidget *node;

	node = new InputNode();
	node->setEnvModel(envModel());
	addNode(node, NODE_INPUT, -300, -200, "Input");

	node = new ScaleNode();
	node->setEnvModel(envModel());
	addNode(node, NODE_SCALE, 0, -250, "Scale");

	node = new CodecNode();
	node->setEnvModel(envModel());
	addNode(node, NODE_CODEC, 0, -100, "Codec");

	node = new MixNode();
	node->setEnvModel(envModel());
	addNode(node, NODE_MIX, -100, -100, "Mix");

	node = new OutputNode();
	node->setEnvModel(envModel());
	addNode(node, NODE_OUTPUT, 300, -200, "Output");

	emit dataChanged(QModelIndex(), QModelIndex());
}

bool NodeGraphModel::LoadGraph(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		ERR_LOG << "Unable to open input file: " << filename.toStdString();
		return false;
	}
	LOG << "Loading graph from: " << filename.toStdString();

	QDataStream in(&file);
	int n = static_cast<int>(m_nodes.size());;
	in >> n;
	for (int i = 0; i < n; ++i)
	{
		int type;
		float x, y;
		QString name;
		in >> type >> x >> y >> name;

		NodeWidget *node = buildNode(type);
		if (!node)
		{
			ERR_LOG << "Invalid node type: " << type << " (in file " << filename.toStdString() << ")";
			return false;
		}

		node->setEnvModel(envModel());
		node->read(in);

		addNode(node, type, x, y, name.toStdString());
	}

	emit dataChanged(QModelIndex(), QModelIndex());
	return true;
}

bool NodeGraphModel::SaveGraph(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		ERR_LOG << "Unable to open output file: " << filename.toStdString();
		return false;
	}
	LOG << "Saving graph to: " << filename.toStdString();

	QDataStream out(&file);
	int n = static_cast<int>(m_nodes.size());
	out << n;
	for (NodeEntry *node : m_nodes)
	{
		out << node->type << node->x << node->y << QString::fromStdString(node->name);
		node->node->write(out);
	}
	return true;
}

void NodeGraphModel::addNode(NodeWidget *nodeWidget, int type, float x, float y, std::string name)
{
	// /!\ not thread-safe
	int nodeIndex = static_cast<int>(m_nodes.size());

	NodeEntry *entry = new NodeEntry();
	entry->node = nodeWidget;
	entry->type = type;
	entry->x = x;
	entry->y = y;
	entry->name = name;

	// links
	entry->inputLinks.resize(nodeWidget->inputSlotsCount());
	entry->outputLinks.resize(nodeWidget->outputSlotsCount());

	// blocks
	entry->nodeIndex.level = NodeLevel;
	entry->nodeIndex.parentNodeIndex = nodeIndex;
	entry->nodeIndex.parentBlockIndex = -1;
	entry->blockIndex.level = BlockLevel;
	entry->blockIndex.parentNodeIndex = nodeIndex;
	entry->blockIndex.parentBlockIndex = -1;
	for (int i = 0; i < _BlockCount; ++i)
	{
		entry->elementIndex[i].level = ElementLevel;
		entry->elementIndex[i].parentNodeIndex = nodeIndex;
		entry->elementIndex[i].parentBlockIndex = i;
	}

	m_nodeLUT[nodeWidget] = nodeIndex;
	m_nodes.push_back(entry);

	nodeWidget->setGraphModel(this);
	nodeWidget->setModelIndex(index(nodeIndex, 0));
	// DANGER! adding nodes provokes memory reallocation! use qt addrow mechanism
}
