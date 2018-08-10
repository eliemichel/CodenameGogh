#ifndef H_SLOTINDEX
#define H_SLOTINDEX

struct SlotIndex
{
	SlotIndex() : node(-1) {}
	SlotIndex(int _node, int _slot) : node(_node), slot(_slot) {}

	int node;
	int slot;

	bool isValid() const { return node != -1; }

	bool operator<(const SlotIndex &other) const { return node < other.node || slot < other.slot; }
};


#endif // H_SLOTINDEX
