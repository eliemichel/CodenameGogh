#ifndef H_QUADTREE
#define H_QUADTREE

#include "UiRect.h"

#include <list>
#include <vector>

struct NVGcontext;

/**
 * Quad tree used for the node area's quick graphic object access
 */
class QuadTree {
public:
	enum Branch {
		NoBranch = -1,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
		_BranchCount,
	};

	struct Accessor {
		std::list<Branch> path;
		union {
			void *data;
			int index;
		};
		bool isValid = false;
	};

	struct Item {
		Item() : isValid(false) {}
		Item(Rect _bbox, void *_data) : bbox(_bbox), data(_data), isValid(true) {}
		Item(Rect _bbox, int _index) : bbox(_bbox), index(_index), isValid(true) {}

		Rect bbox;
		union {
			void *data;
			int index;
		};
		bool isValid;
	};

public:
	QuadTree(float cx, float cy, float hw, float hh, int divisions);

	~QuadTree();

	bool IsLeaf() const { return m_branches[0] == nullptr; }

	/// Insert at the deepest possible node, branching while the division limit has not been reached
	Accessor Insert(Item item);

	Accessor ItemAt(float x, float y);

	/// Pop items matching the provided data
	void RemoveItems(const std::vector<Accessor> & accessors);
	void RemoveItem(const Accessor & acc) { RemoveItems({ acc }); }

	Accessor UpdateItemBBox(const Accessor & acc, Rect bbox);

	void PaintDebug(NVGcontext *vg) const;

private:
	void Split();

	/// Prune unused children
	void Prune();

	/// If the item fits in one of the children, return a pointer to this
	/// child, otherwise return nullptr.
	Branch FitInBranch(Item item);

private:
	/// Center
	float m_cx, m_cy;
	/// Half width and height
	float m_hw, m_hh;
	/// The maximum number of potential subdivisions, decreasing for children
	int m_divisions;

	/// Children, always all null or all non null
	QuadTree *m_branches[_BranchCount];
	/// Items that cannot fit in children, whether it is because there is no
	/// child tree or because the item's bbox is too large.
	std::vector<Item> m_items;
};

#endif //  H_QUADTREE
