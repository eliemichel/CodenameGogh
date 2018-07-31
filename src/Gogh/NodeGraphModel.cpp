#include "NodeGraphModel.h"

#include "Logger.h"

#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"

#include <QFile>
#include <QDataStream>
#include <QModelIndex>

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

bool NodeGraphModel::isNodeIndex(const QModelIndex & index) const
{
	return isRoot(index.parent()) && inParentBounds(index);
}

QModelIndex NodeGraphModel::index(int row, int column, const QModelIndex & parent) const
{
	if (row < 0 || row >= rowCount(parent) || column < 0 || column >= columnCount(parent))
	{
		return QModelIndex();
	}
	
	if (isRoot(parent))
	{
		return createIndex(row, column, -1);
	}
	else if (isNodeIndex(parent))
	{
		NodeEntry entry = nodes()[parent.row()];
		return createIndex(row, column, parent.row());
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex NodeGraphModel::parent(const QModelIndex & index) const
{
	if (!index.isValid() || index.internalId() == -1)
	{
		return QModelIndex();
	}
	else
	{
		return createIndex(index.internalId(), 0, -1);
	}
}

int NodeGraphModel::rowCount(const QModelIndex & parent) const
{
	if (isRoot(parent))
	{
		return static_cast<int>(m_nodes.size());
	}
	else if (isNodeIndex(parent))
	{
		NodeEntry entry = nodes()[parent.row()];
		return entry.node->parmCount();
	}
	else
	{
		return 0;
	}
}

int NodeGraphModel::columnCount(const QModelIndex & parent) const
{
	if (isRoot(parent))
	{
		return _ColumnCount;
	}
	else if (isNodeIndex(parent))
	{
		return 2; // parmName and parmEval
	}
	else
	{
		return 0;
	}
}

QVariant NodeGraphModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::WhatsThisRole || role == Qt::UserRole)
	{
		if (isRoot(index.parent()))
		{
			NodeEntry entry = nodes()[index.row()];
			switch (index.column())
			{
			case TypeColumn:
				if (role == Qt::UserRole)
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
			default:
				return QVariant();
			}
		}
		else if (isNodeIndex(index.parent()))
		{
			NodeEntry entry = nodes()[index.internalId()];
			switch (index.column())
			{
			case 0:
				return entry.node->parmName(index.row());
			case 1:
				return entry.node->parmEval(index.row());
			default:
				return QVariant();
			}
		}
	}
	else
	{
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
		default:
			return QVariant();
		}
	}
	else
	{
		return QVariant();
	}
}

void NodeGraphModel::LoadDefaultGraph()
{
	addNode(new InputNode(), NODE_INPUT, -300, -200);
	addNode(new ScaleNode(), NODE_SCALE, 0, -250);
	addNode(new CodecNode(), NODE_CODEC, -0, -100);
	addNode(new OutputNode(), NODE_OUTPUT, 300, -200);
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
	size_t n;
	in >> n;
	for (int i = 0; i < n; ++i)
	{
		int type;
		float x, y;
		in >> type >> x >> y;

		NodeWidget *node = buildNode(type);
		if (!node)
		{
			ERR_LOG << "Invalid node type: " << type << " (in file " << filename.toStdString() << ")";
			return false;
		}

		node->read(in);

		addNode(node, type, x, y);
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
	size_t n = nodes().size();
	out << n;
	for (NodeEntry entry : nodes())
	{
		out << entry.type << entry.x << entry.y;
		entry.node->write(out);
	}
	return true;
}

void NodeGraphModel::addNode(NodeWidget *node, int type, float x, float y)
{
	NodeEntry entry;
	entry.node = node;
	entry.type = type;
	entry.x = x;
	entry.y = y;
	m_nodes.push_back(entry);
}
