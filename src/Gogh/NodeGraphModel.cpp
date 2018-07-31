#include "NodeGraphModel.h"

#include "Logger.h"

#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"

#include <QFile>
#include <QDataStream>

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

void NodeGraphModel::LoadDefaultGraph()
{
	addNode(new InputNode(), NODE_INPUT, -300, -200);
	addNode(new ScaleNode(), NODE_SCALE, 0, -250);
	addNode(new CodecNode(), NODE_CODEC, -0, -100);
	addNode(new OutputNode(), NODE_OUTPUT, 300, -200);
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
