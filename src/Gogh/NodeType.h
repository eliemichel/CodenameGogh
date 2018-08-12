#ifndef H_NODETYPE
#define H_NODETYPE

#include <string>
#include <vector>

class Node;

/**
 * Node factory
 */
class NodeType
{
public:
	// When adding a new type here, handle it in the body of create(), info(), name(), etc.
	enum NodeTypeId
	{
		NODE_OUTPUT = 0,
		NODE_INPUT = 1,
		NODE_SCALE = 2,
		NODE_CODEC = 3,
		NODE_MIX = 4,
		_NodeTypeCount, // special item, must remain the last one
	};

public:
	static std::vector<NodeType> availableTypes();

public:
	NodeType(NodeTypeId typeId);
	NodeType(int typeId);

	/**
	 * Display name un menus
	 */
	std::string name() const;

	/**
	 * Name used for technical views
	 */
	std::string systemName() const;

	/**
	 * Information about the node type
	 */
	std::string info() const;

	/**
	 * Create a new node of this type. You must delete it yourself
	 */
	Node * create() const;

private:
	NodeTypeId m_typeId;
};

#endif // H_NODETYPE
