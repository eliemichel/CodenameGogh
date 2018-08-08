#include "NodeGraphModel.h"

#include "Logger.h"

#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"

#include <QFile>
#include <QDataStream>
#include <QModelIndex>

NodeGraphModel::NodeGraphModel()
	: QAbstractItemModel()
	, m_envModel(nullptr)
{}

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
	default:
		return nullptr;
	}
}

bool NodeGraphModel::isRoot(QModelIndex index)
{
	return !index.isValid();
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
	Node *node = data ? nodes()[data->parentNodeIndex].node : nullptr;
	switch (level)
	{

	case RootLevel:
		return createIndex(row, column, &m_nodes[row].nodeIndex);

	case NodeLevel:
		return createIndex(row, column, &m_nodes[parent.row()].blockIndex);

	case BlockLevel:
		return createIndex(row, column, &m_nodes[data->parentNodeIndex].elementIndex[parent.row()]);

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
		NodeEntry & entry = m_nodes[data->parentNodeIndex];
		switch (data->level)
		{
		case NodeLevel:
			return QModelIndex();
		case BlockLevel:
			return createIndex(data->parentNodeIndex, 0, &entry.nodeIndex);
		case ElementLevel:
			return createIndex(data->parentBlockIndex, 0, &entry.blockIndex);
		default:
			return QModelIndex();
		}
	}
}

int NodeGraphModel::rowCount(const QModelIndex & parent) const
{
	IndexData *data = indexData(parent);
	IndexLevel level = data ? data->level : RootLevel;
	Node *node = data ? nodes()[data->parentNodeIndex].node : nullptr;
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
			return node->parmCount();
		case InputSlotBlock:
			return node->inputSlotsCount();
		case OutputSlotBlock:
			return node->outputSlotsCount();
		}

	default:
		return 0;

	}
}

int NodeGraphModel::columnCount(const QModelIndex & parent) const
{
	IndexData *data = indexData(parent);
	IndexLevel level = data ? data->level : RootLevel;
	Node *node = data ? nodes()[data->parentNodeIndex].node : nullptr;
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
	Node *node = data ? nodes()[data->parentNodeIndex].node : nullptr;
	switch (level)
	{

	case RootLevel:
		return QVariant();

	case NodeLevel:
	{
		NodeEntry entry = nodes()[index.row()];
		switch (index.column())
		{
		case TypeColumn:
			if (role == Qt::EditRole)
			{
				return entry.type;
			}
			else
			{
				return QString::fromStdString(nodeTypeToString(entry.type));
			}
		case PosXColumn:
			return entry.x;
		case PosYColumn:
			return entry.y;
		case NameColumn:
			return QString::fromStdString(entry.name);
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
				return node->parmName(index.row());
			case 1: // value
				return node->parmEval(index.row());
			default:
				return QVariant();
			}
		case InputSlotBlock:
		{
			/*
			if (const Slot *slot = node->inputSlots()[index.row()])
			{
				if (const Slot *sourceSlot = slot->sourceSlot())
				{
					if (const Node *sourceNode = sourceSlot->parentNode())
					{
						int nodeId = m_nodeLUT.at(sourceNode);
						int slotId = sourceNode->outputSlotIndex(sourceSlot);
						if (role == Qt::EditRole)
						{
							switch (index.column())
							{
							case 0: // node
								return nodeId;
							case 1: // slot
								return slotId;
							default:
								return QVariant();
							}
						}
						else
						{
							switch (index.column())
							{
							case 0: // node
								return QString("Node #") + QString::number(nodeId);
							case 1: // slot
								return QString("Output Slot #") + QString::number(slotId);
							default:
								return QVariant();
							}
						}
					}
				}
			}
			return index.column() == 0 ? "<not connected>" : QVariant();
			*/

			SlotIndex & sourceSlot = m_nodes[data->parentNodeIndex].inputLinks[index.row()];
			
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
				if (!sourceSlot.isConnected())
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
			SlotIndex & sourceSlot = m_nodes[data->parentNodeIndex].outputLinks[index.row()];

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
				if (!sourceSlot.isConnected())
				{
					return index.column() == 0 ? "<not connected>" : QVariant();
				}

				switch (index.column())
				{
				case 0: // node
					return QString("Node #") + QString::number(sourceSlot.node);
				case 1: // slot
					return QString("Input Slot #") + QString::number(sourceSlot.slot);
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
		NodeEntry & entry = m_nodes[index.row()];
		switch (index.column())
		{
		case PosXColumn:
			entry.x = value.toFloat();
			break;
		case PosYColumn:
			entry.y = value.toFloat();
			break;
		case NameColumn:
			entry.name = value.toString().toStdString();
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
		Node * node = data ? nodes()[data->parentNodeIndex].node : nullptr;
		switch (index.parent().row())
		{
		case ParamBlock:
			switch (index.column())
			{
			case 1: // value
				node->setParm(index.row(), value);
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

			SlotIndex & sourceSlot = m_nodes[data->parentNodeIndex].inputLinks[index.row()];

			switch (index.column())
			{
			case 0: // node
			{
				if (id < 0 || id >= nodes().size() || id == sourceSlot.node)
				{
					return false;
				}

				const QModelIndex & oldSourceIndex = this->index(sourceSlot.node, 0);
				const QModelIndex & newSourceIndex = this->index(id, 0);
				const QModelIndex & nodeIndex = this->index(data->parentNodeIndex, 0);

				// deconnect previous parent
				bool wasConnected = sourceSlot.isConnected();
				if (wasConnected)
				{
					m_nodes[sourceSlot.node].outputLinks[sourceSlot.slot].node = -1;
				}

				// update
				int n = m_nodes[id].outputLinks.size();
				if (n == 0)
				{
					return false;
				}
				sourceSlot.node = id;
				sourceSlot.slot = std::max(0, std::min(sourceSlot.slot, n - 1));

				// register to new parent
				SlotIndex & destinationSlot = m_nodes[sourceSlot.node].outputLinks[sourceSlot.slot];
				destinationSlot.node = data->parentNodeIndex;
				destinationSlot.slot = index.row();

				// signal changes
				if (wasConnected)
				{
					emit dataChanged(oldSourceIndex, oldSourceIndex);
				}
				emit dataChanged(newSourceIndex, newSourceIndex);
				emit dataChanged(nodeIndex, nodeIndex);
				return true;
			}
			case 1: // slot
			{
				if (!sourceSlot.isConnected())
				{
					return false;
				}

				int n = m_nodes[sourceSlot.node].outputLinks.size();
				if (id < 0 || id >= n)
				{
					return false;
				}

				const QModelIndex & sourceIndex = this->index(sourceSlot.node, 0);
				const QModelIndex & nodeIndex = this->index(data->parentNodeIndex, 0);

				// deconnect previous parent
				m_nodes[sourceSlot.node].outputLinks[sourceSlot.slot].node = -1;

				// update
				sourceSlot.slot = id;

				// register to new parent
				SlotIndex & destinationSlot = m_nodes[sourceSlot.node].outputLinks[sourceSlot.slot];
				destinationSlot.node = data->parentNodeIndex;
				destinationSlot.slot = index.row();

				emit dataChanged(sourceIndex, sourceIndex);
				emit dataChanged(nodeIndex, nodeIndex);
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
	for (NodeEntry entry : nodes())
	{
		out << entry.type << entry.x << entry.y << QString::fromStdString(entry.name);
		entry.node->write(out);
	}
	return true;
}

void NodeGraphModel::addNode(Node *node, int type, float x, float y, std::string name)
{
	// /!\ not thread-safe
	int nodeIndex = static_cast<int>(m_nodes.size());

	NodeEntry entry;
	entry.node = node;
	entry.type = type;
	entry.x = x;
	entry.y = y;
	entry.name = name;

	// links
	for (int i = 0; i < node->inputSlotsCount(); ++i)
	{
		entry.inputLinks.push_back(SlotIndex());
	}
	for (int i = 0; i < node->outputSlotsCount(); ++i)
	{
		entry.outputLinks.push_back(SlotIndex());
	}

	// blocks
	entry.nodeIndex.level = NodeLevel;
	entry.nodeIndex.parentNodeIndex = nodeIndex;
	entry.nodeIndex.parentBlockIndex = -1;
	entry.blockIndex.level = BlockLevel;
	entry.blockIndex.parentNodeIndex = nodeIndex;
	entry.blockIndex.parentBlockIndex = -1;
	for (int i = 0; i < _BlockCount; ++i)
	{
		entry.elementIndex[i].level = ElementLevel;
		entry.elementIndex[i].parentNodeIndex = nodeIndex;
		entry.elementIndex[i].parentBlockIndex = i;
	}

	m_nodeLUT[node] = nodeIndex;
	m_nodes.push_back(entry);
}
