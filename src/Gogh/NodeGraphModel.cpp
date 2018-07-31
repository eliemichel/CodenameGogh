#include "NodeGraphModel.h"

#include "Logger.h"

#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"

#include <QFile>
#include <QDataStream>

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
	size_t n;
	in >> n;
	for (int i = 0; i < n; ++i)
	{
		int type;
		float x, y;
		in >> type >> x >> y;

		NodeWidget *node;
		switch (type)
		{
		case NODE_INPUT:
			node = new InputNode();
			break;
		case NODE_OUTPUT:
			node = new OutputNode();
			break;
		case NODE_SCALE:
			node = new ScaleNode();
			break;
		case NODE_CODEC:
			node = new CodecNode();
			break;
		default:
			ERR_LOG << "Invalid node type: " << type << " (in file " << filename.toStdString() << ")";
			return false;
		}
		addNode(node, type, x, y);
	}
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
	}
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
