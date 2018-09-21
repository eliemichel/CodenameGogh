#include "NodeType.h"
#include "Logger.h"

#include "Node.h"
#include "Nodes/InputNode.h"
#include "Nodes/OutputNode.h"
#include "Nodes/ScaleNode.h"
#include "Nodes/CodecNode.h"
#include "Nodes/MixNode.h"
#include "Nodes/FramerateNode.h"

std::vector<NodeType> NodeType::availableTypes()
{
	return std::vector<NodeType>{
		NodeType(NODE_OUTPUT),
		NodeType(NODE_INPUT),
		NodeType(NODE_SCALE),
		NodeType(NODE_CODEC),
		NodeType(NODE_MIX),
		NodeType(NODE_FRAMERATE),
	};
}

NodeType::NodeType(NodeTypeId typeId)
	: m_typeId(typeId)
{}

NodeType::NodeType(int typeId)
{
	if (typeId < 0 || typeId >= _NodeTypeCount)
	{
		ERR_LOG << "Invalid node type id: " << m_typeId;
	}
	m_typeId = static_cast<NodeTypeId>(typeId);
}

std::string NodeType::name() const
{
	switch (m_typeId)
	{
	case NODE_INPUT:
		return "Input";
	case NODE_OUTPUT:
		return "Output";
	case NODE_SCALE:
		return "Scale";
	case NODE_CODEC:
		return "Codec";
	case NODE_MIX:
		return "Mix";
	case NODE_FRAMERATE:
		return "Framerate";
	default:
		WARN_LOG << "Unknown node type id: " << m_typeId;
		return "<unknown type>";
	}
}

std::string NodeType::systemName() const
{
	switch (m_typeId)
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
	case NODE_FRAMERATE:
		return "NODE_FRAMERATE";
	default:
		WARN_LOG << "Unknown node type id: " << m_typeId;
		return "<unknown type>";
	}
}

std::string NodeType::info() const
{
	switch (m_typeId)
	{
	case NODE_INPUT:
		return "Read input file";
	case NODE_OUTPUT:
		return "Write output file";
	case NODE_SCALE:
		return "Change video stream dimensions";
	case NODE_CODEC:
		return "Change streams codec";
	case NODE_MIX:
		return "Mix streams";
	case NODE_FRAMERATE:
		return "Set output framerate";
	default:
		WARN_LOG << "Unknown node type id: " << m_typeId;
		return "<unknown type>";
	}
}

Node * NodeType::create() const
{
	Node *node = nullptr;

	switch (m_typeId)
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
	case NODE_MIX:
		node = new MixNode();
		break;
	case NODE_FRAMERATE:
		node = new FramerateNode();
		break;
	default:
		WARN_LOG << "Unknown node type id: " << m_typeId;
		break;
	}

	if (node)
	{
		node->setType(m_typeId);
	}
	return node;
}
