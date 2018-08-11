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
	while (!m_nodeEntries.empty())
	{
		NodeEntry *entry = m_nodeEntries.back();
		delete entry->node;
		delete entry;
		m_nodeEntries.pop_back();
	}
}

Node * NodeGraphModel::buildNode(int type)
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

bool NodeGraphModel::canAddLink(const SlotIndex & origin, const SlotIndex & destination)
{
	if (origin.node < 0 || origin.node >= m_nodeEntries.size() || destination.node < 0 || destination.node>= m_nodeEntries.size())
	{
		return false;
	}
	const Node *originNode = m_nodeEntries[origin.node]->node;
	const Node *destinationNode = m_nodeEntries[destination.node]->node;
	if (origin.slot < 0 || origin.slot >= originNode->outputLinks.size() || destination.slot < 0 || destination.slot >= destinationNode->inputLinks.size())
	{
		return false;
	}
	// TODO: check for graph loops
	return true;
}

bool NodeGraphModel::addLink(const SlotIndex & origin, const SlotIndex & destination)
{
	if (!canAddLink(origin, destination))
	{
		return false;
	}
	Node *originNode = node(origin.node);
	Node *destinationNode = node(destination.node);

	// unplug previous link
	SlotIndex oldOrigin = destinationNode->inputLinks[destination.slot];
	bool wasConnected = oldOrigin.isValid();
	if (wasConnected)
	{
		Node *oldOriginEntry = node(oldOrigin.node);
		oldOriginEntry->outputLinks[oldOrigin.slot].erase(destination);
	}

	// plug new link
	originNode->outputLinks[origin.slot].insert(destination);
	destinationNode->inputLinks[destination.slot] = origin;

	// signal change to views
	const QModelIndex & oldSourceIndex = index(oldOrigin.node, 0);
	if (wasConnected)
	{
		emit dataChanged(oldSourceIndex, oldSourceIndex);
	}
	const QModelIndex & newSourceIndex = index(origin.node, 0);
	const QModelIndex & destinationIndex = index(destination.node, 0);
	emit dataChanged(newSourceIndex, newSourceIndex);
	emit dataChanged(destinationIndex, destinationIndex);

	// signal change to nodes
	if (wasConnected)
	{
		Node *oldOriginEntry = node(oldOrigin.node);
		oldOriginEntry->fireSlotDisconnectEvent(oldOrigin.slot, false /* isInput */);
	}
	originNode->fireSlotConnectEvent(origin.slot, false /* isInput */);
	destinationNode->fireSlotConnectEvent(destination.slot, true /* isInput */);
	
	return true;
}

bool NodeGraphModel::removeLink(const SlotIndex & destination)
{
	if (destination.node < 0 || destination.node >= m_nodeEntries.size())
	{
		DEBUG_LOG << "invalid destination node";
		return false;
	}

	Node *destinationNode = node(destination.node);
	if (destination.slot < 0 || destination.slot >= destinationNode->inputLinks.size())
	{
		DEBUG_LOG << "invalid destination slot: #" << destination.slot << " (in node #" << destination.node << ")";
		return false;
	}

	SlotIndex & origin = destinationNode->inputLinks[destination.slot];
	if (!origin.isValid())
	{
		DEBUG_LOG << "slot had no connection";
		return false;
	}

	// remove link
	Node *originNode = node(origin.node);
	originNode->outputLinks[origin.slot].erase(destination);
	origin.node = -1;

	// signal views
	const QModelIndex & originIndex = index(origin.node, 0);
	const QModelIndex & destinationIndex = index(destination.node, 0);
	emit dataChanged(originIndex, originIndex);
	emit dataChanged(destinationIndex, destinationIndex);

	// signal node
	originNode->fireSlotDisconnectEvent(origin.slot, false /* isInput */);
	destinationNode->fireSlotDisconnectEvent(destination.slot, true /* isInput */);

	return true;
}

const SlotIndex & NodeGraphModel::originSlot(const SlotIndex & destination) const
{
	if (destination.node < 0 || destination.node >= m_nodeEntries.size())
	{
		WARN_LOG << "Invalid destination node: #" << destination.node;
		return invalidSlot;
	}
	const Node *destinationNode = node(destination.node);
	if (destination.slot < 0 || destination.slot >= destinationNode->inputLinks.size())
	{
		WARN_LOG << "Invalid destination slot: #" << destination.slot << " (in node #" << destination.node << ")";
		return invalidSlot;
	}
	return destinationNode->inputLinks[destination.slot];
}

const std::set<SlotIndex> & NodeGraphModel::destinationSlots(const SlotIndex & origin) const
{
	if (origin.node < 0 || origin.node >= m_nodeEntries.size())
	{
		WARN_LOG << "Invalid source node: #" << origin.node;
		return invalidSlotSet;
	}
	const Node *originNode = node(origin.node);
	if (origin.slot < 0 || origin.slot >= originNode->outputLinks.size())
	{
		WARN_LOG << "Invalid source slot: #" << origin.slot << " (in node #" << origin.node << ")";
		return invalidSlotSet;
	}
	return originNode->outputLinks[origin.slot];
}

QModelIndex NodeGraphModel::findByName(const std::string & name)
{
	int n = rowCount();
	for (int i = 0 ; i < n ; ++i)
	{
		if (node(i)->name == name)
		{
			return index(i, 0);
		}
	}
	return QModelIndex();
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
		return createIndex(row, column, &m_nodeEntries[row]->nodeIndex);

	case NodeLevel:
		return createIndex(row, column, &m_nodeEntries[parent.row()]->blockIndex);

	case BlockLevel:
		return createIndex(row, column, &m_nodeEntries[data->parentNodeIndex]->elementIndex[parent.row()]);

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
		NodeEntry *node = m_nodeEntries[data->parentNodeIndex];
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
	switch (level)
	{

	case RootLevel:
		return static_cast<int>(m_nodeEntries.size());

	case NodeLevel:
		return _BlockCount;

	case BlockLevel:
		switch (parent.row())
		{
		case ParamBlock:
			return node(data->parentNodeIndex)->parmCount();
		case InputSlotBlock:
			return static_cast<int>(node(data->parentNodeIndex)->inputLinks.size());
		case OutputSlotBlock:
			return static_cast<int>(node(data->parentNodeIndex)->outputLinks.size());
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
			return 3; // parmName, parmType and parmEval
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
	switch (level)
	{

	case RootLevel:
		return QVariant();

	case NodeLevel:
	{
		Node *node = this->node(index.row());
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
				return node(data->parentNodeIndex)->parmName(index.row());
			case 1: // type
				return node(data->parentNodeIndex)->parmType(index.row());
			case 2: // value
				return node(data->parentNodeIndex)->parmRawValue(index.row());
			default:
				return QVariant();
			}
		case InputSlotBlock:
		{
			SlotIndex sourceSlot = node(data->parentNodeIndex)->inputLinks[index.row()];
			
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
			const std::set<SlotIndex> & destinations = node(data->parentNodeIndex)->outputLinks[index.row()];
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
		Node *node = this->node(index.row());
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
		switch (index.parent().row())
		{
		case ParamBlock:
			switch (index.column())
			{
			case 2: // value
				node(data->parentNodeIndex)->setParm(index.row(), value);
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

			SlotIndex & sourceSlot = node(data->parentNodeIndex)->inputLinks[index.row()];

			switch (index.column())
			{
			case 0: // node
			{
				if (id < 0 || id >= nodes().size() || id == sourceSlot.node)
				{
					return false;
				}

				// clamp slot
				int n = static_cast<int>(node(id)->outputLinks.size());
				if (n == 0)
				{
					return false;
				}
				int slot = std::max(0, std::min(sourceSlot.slot, n - 1));

				addLink(SlotIndex(id, slot), SlotIndex(data->parentNodeIndex, index.row()));

				return true;
			}
			case 1: // slot
			{
				if (!sourceSlot.isValid())
				{
					return false;
				}

				int n = static_cast<int>(node(sourceSlot.node)->outputLinks.size());
				if (id < 0 || id >= n)
				{
					return false;
				}

				addLink(SlotIndex(sourceSlot.node, id), SlotIndex(data->parentNodeIndex, index.row()));

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
			case 2: // value
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
	Node *node;

	node = new InputNode();
	node->type = NODE_INPUT;
	node->x = -300;
	node->y = -200;
	node->name = "Input";
	addNode(node);

	node = new ScaleNode();
	node->type = NODE_SCALE;
	node->x = 0;
	node->y = -250;
	node->name = "Scale";
	addNode(node);

	node = new CodecNode();
	node->type = NODE_CODEC;
	node->x = 0;
	node->y = -100;
	node->name = "Codec";
	addNode(node);

	node = new MixNode();
	node->type = NODE_MIX;
	node->x = -100;
	node->y = -100;
	node->name = "Mix";
	addNode(node);
	
	node = new OutputNode();
	node->type = NODE_OUTPUT;
	node->x = 300;
	node->y = -200;
	node->name = "Output";
	addNode(node);

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

	// Nodes
	int n;
	in >> n;
	for (int i = 0; i < n; ++i)
	{
		int type;
		float x, y;
		QString name;
		in >> type >> x >> y >> name;

		Node *node = buildNode(type);
		if (!node)
		{
			ERR_LOG << "Invalid node type: " << type << " (in file " << filename.toStdString() << ")";
			return false;
		}

		node->type = type;
		node->x = x;
		node->y = y;
		node->name = name.toStdString();
		node->read(in);
		
		addNode(node);
	}

	// Links
	for (int i = 0; i < n; ++i)
	{
		int maxM = static_cast<int>(node(i)->inputSlotCount());
		int m;
		in >> m;
		if (m > maxM)
		{
			ERR_LOG << "Invalid number of input links: " << m << " (" << maxM << " was expected, in node #" << i << ", in file " << filename.toStdString() << ")";
		}
		for (int j = 0; j < m; ++j)
		{
			SlotIndex destination(i, j);
			SlotIndex origin;
			in >> origin.node >> origin.slot;
			addLink(origin, destination);
		}
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

	// Nodes
	int n = static_cast<int>(m_nodeEntries.size());
	out << n;
	for (NodeEntry *entry : m_nodeEntries)
	{
		Node *node = entry->node;
		out << node->type << node->x << node->y << QString::fromStdString(node->name);
		node->write(out);
	}

	// Links
	for (NodeEntry *entry : m_nodeEntries)
	{
		Node *node = entry->node;
		out << node->inputSlotCount();
		for (const SlotIndex & origin : node->inputLinks)
		{
			out << origin.node << origin.slot;
		}
	}

	return true;
}

void NodeGraphModel::addNode(Node *node)
{
	// /!\ not thread-safe
	int nodeIndex = static_cast<int>(m_nodeEntries.size());

	NodeEntry *entry = new NodeEntry();
	entry->node = node;

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

	m_nodeEntries.push_back(entry);

	node->setEnvModel(envModel());
	node->setGraphModel(this);
	node->setModelIndex(index(nodeIndex, 0));
}
