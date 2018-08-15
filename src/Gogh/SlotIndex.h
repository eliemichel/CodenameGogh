#ifndef H_SLOTINDEX
#define H_SLOTINDEX

class Node;

struct SlotIndex
{
	SlotIndex() : node(-1) {}
	SlotIndex(int _node, int _slot) : node(_node), slot(_slot) {}

	int node;
	int slot;

	bool isValid() const { return node != -1; }

	bool operator<(const SlotIndex &other) const { return node < other.node || (node == other.node && slot < other.slot); }
};


/**
 * Same as SlotIndex but referencing node by pointer instead of numerical index
 */
struct Connection
{
	Connection() : node(nullptr) {}
	Connection(Node *_node, int _slot) : node(_node), slot(_slot) {}

	Node *node;
	int slot;

	bool isValid() const { return node != nullptr; }

	bool operator<(const Connection &other) const { return node < other.node || (node == other.node && slot < other.slot); }
};


#endif // H_SLOTINDEX
