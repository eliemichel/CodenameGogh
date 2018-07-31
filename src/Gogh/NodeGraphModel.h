#ifndef H_NODEGRAPHMODEL
#define H_NODEGRAPHMODEL

#include "NodeWidget.h"

#include <string>
#include <vector>

#include <QString> // should be replaced by a stream path class

class NodeGraphModel
{
public:
	// This is a temprary structure, while we don't have a proper Node type
	struct NodeEntry
	{
		NodeWidget *node;
		int type;
		float x, y;
	};

	enum NodeType
	{
		NODE_OUTPUT = 0,
		NODE_INPUT  = 1,
		NODE_SCALE  = 2,
		NODE_CODEC  = 3,
	};

public:
	void LoadDefaultGraph();
	bool LoadGraph(QString filename);
	bool SaveGraph(QString filename);

	void addNode(NodeWidget *node, int type, float x, float y);
	const std::vector<NodeEntry> & nodes() const { return m_nodes; }

private:
	/**
	 * This should be a vector of non GUI objects, but for now we don't have a
	 * node widget generator from node definition, and I want to keep it simple
	 * to add new node type
	 */
	std::vector<NodeEntry> m_nodes;
};

#endif // H_NODEGRAPHMODEL