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

	class Item {
	public:
		Item(Rect bbox, int type)
			: m_bbox(bbox)
			, m_type(type)
			, m_layer(0.f)
		{}

		int Type() const { return m_type; }

		Rect BBox() const { return m_bbox; }
		void SetBBox(const Rect & bbox) {
			m_bbox = bbox;
			UpdateGeometry();
		}

		float Layer() const { return m_layer; }
		void SetLayer(float layer) { m_layer = layer; }

		/// x and y are garanteed to be within the bounding box
		virtual bool Hit(float x, float y) { return true; }

	protected:
		/// Called to update item geometry after bbox changed
		virtual void UpdateGeometry() {}

	private:
		QuadTree *m_tree;
		Rect m_bbox;
		int m_type;
		float m_layer;
	};

	class Accessor {
	public:
		Accessor() : isValid(false) {}
		Accessor(Item *_item) : item(_item), isValid(true) {}

		bool IsValid() const { return isValid; }

		// TODO: make private
		std::list<Branch> path;
		Item *item;
		bool isValid = false;
	};

public:
	QuadTree(float cx, float cy, float hw, float hh, int divisions);

	~QuadTree();

	bool IsLeaf() const { return m_branches[0] == nullptr; }

	/// Insert at the deepest possible node, branching while the division limit
	/// has not been reached
	Accessor Insert(Item *item);

	/// Try to build an accessor to the item, or return an invalid accessor if
	/// not found
	Accessor Find(Item *item);

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
	Branch FitInBranch(Item *item);

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
	/// TODO: replace with a set
	std::vector<Item*> m_items;
};

#endif //  H_QUADTREE
