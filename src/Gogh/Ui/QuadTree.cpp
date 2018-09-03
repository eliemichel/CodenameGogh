#include "QuadTree.h"

#include <nanovg.h>

QuadTree::QuadTree(float cx, float cy, float hw, float hh, int divisions)
	: m_cx(cx)
	, m_cy(cy)
	, m_hw(hw)
	, m_hh(hh)
	, m_divisions(divisions)
{
	for (int i = 0; i < _BranchCount; ++i) {
		m_branches[i] = nullptr;
	}
}

QuadTree::~QuadTree() {
	if (!IsLeaf()) {
		for (int i = 0; i < _BranchCount; ++i) {
			delete m_branches[i];
		}
	}
}

QuadTree::Accessor QuadTree::Insert(Item *item, bool notify) {
	if (!item) {
		return Accessor();
	}

	Branch branchIndex = FitInBranch(item);
	if (branchIndex != NoBranch) {
		if (IsLeaf()) {
			Split();
		}
		Accessor acc = m_branches[branchIndex]->Insert(item, false /* notify */);
		if (notify) {
			item->OnInsert(this);
		}
		acc.path.push_back(branchIndex);
		return acc;
	} else {
		m_items.push_back(item);
		if (notify) {
			item->OnInsert(this);
		}
		return Accessor(item);
	}
}

QuadTree::Accessor QuadTree::Find(Item *item) {
	if (!item) {
		return Accessor();
	}

	Branch branchIndex = FitInBranch(item);
	if (branchIndex != NoBranch && !IsLeaf()) {
		Accessor acc = m_branches[branchIndex]->Find(item);
		acc.path.push_back(branchIndex);
		return acc;
	} else {
		for (Item *other : m_items) {
			if (item == other) {
				return Accessor(other);
			}
		}
	}

	return Accessor();
}

QuadTree::Accessor QuadTree::ItemAt(float x, float y) {
	Accessor candidate = Accessor();
	float candidateLayer;

	for (Item *item : m_items) {
		if (item->BBox().Contains(x, y) && item->Hit(x, y)) {
			float itemLayer = item->Layer();
			if (!candidate.isValid || itemLayer > candidateLayer) {
				candidate.isValid = true;
				candidate.item = item;
				candidateLayer = itemLayer;
			}
		}
	}

	if (!IsLeaf()) {
		Branch branch = (
			x < m_cx
			? (y < m_cy ? TopLeft : BottomLeft)
			: (y < m_cy ? TopRight : BottomRight)
			);

		Accessor acc = m_branches[branch]->ItemAt(x, y);
		acc.path.push_back(branch);

		if (acc.isValid && (!candidate.isValid || acc.item->Layer() > candidateLayer)) {
			candidate = acc;
		}
	}

	return candidate;
}

void QuadTree::RemoveItems(const std::vector<Accessor> & accessors, bool notify) {
	// split item lists
	std::vector<Accessor> subAccessors[_BranchCount];

	for (Accessor acc : accessors) {
		if (!acc.isValid) {
			continue;
		}
		if (acc.path.empty()) {
			// search in items
			for (auto it = m_items.begin(); it != m_items.end();) {
				if (*it == acc.item) {
					it = m_items.erase(it);
				}
				else {
					++it;
				}
			}
		} else {
			Branch branchIndex = acc.path.back();
			acc.path.pop_back();
			subAccessors[branchIndex].push_back(acc);
		}
	}

	if (!IsLeaf()) {
		for (int i = 0; i < _BranchCount; ++i) {
			if (!subAccessors[i].empty()) {
				m_branches[i]->RemoveItems(subAccessors[i], false /* notify */);
			}
		}
	}

	Prune();

	if (notify) {
		for (Accessor acc : accessors) {
			acc.item->OnRemove(this);
		}
	}
}

QuadTree::Accessor QuadTree::UpdateItemBBox(const Accessor & acc, Rect bbox) {
	Accessor newAcc = acc;
	newAcc.isValid = false;

	// update recursively
	if (!newAcc.path.empty()) {
		Branch branchIndex = newAcc.path.back();
		newAcc.path.pop_back();

		if (!IsLeaf()) {
			newAcc = m_branches[branchIndex]->UpdateItemBBox(newAcc, bbox);
			if (newAcc.isValid) {
				newAcc.path.push_back(branchIndex);
			}
		}
	}

	if (!newAcc.isValid) {
		bool fit = bbox.x >= m_cx - m_hw && bbox.x + bbox.w <= m_cx + m_hw
			&& bbox.y >= m_cy - m_hh && bbox.y + bbox.h <= m_cy + m_hh;

		// remove from items
		newAcc.isValid = true;
		RemoveItem(newAcc, false /* notify */);

		// re-insert if possible, potentially inserting deeper
		if (fit && newAcc.item) {
			newAcc.item->SetBBox(bbox);
			newAcc = Insert(newAcc.item, false /* notify */);
		}

		// Flag invalid if new bbox does not fit for parent to re-insert the item
		newAcc.isValid = fit;
	}

	return newAcc;
}

void QuadTree::PaintDebug(NVGcontext *vg) const {
	nvgBeginPath(vg);
	nvgRect(vg, m_cx - m_hw, m_cy - m_hh, 2.f * m_hw, 2.f * m_hh);
	nvgFillColor(vg, nvgRGBA(0, 255, 0, 32));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRect(vg, m_cx - m_hw + .5f, m_cy - m_hh + .5f, 2.f * m_hw - 1.f, 2.f * m_hh - 1.f);
	nvgStrokeColor(vg, nvgRGBA(0, 255, 0, 32));
	nvgStroke(vg);

	if (!IsLeaf()) {
		for (int i = 0; i < _BranchCount; ++i) {
			m_branches[i]->PaintDebug(vg);
		}
	}

	for (const Item * item : m_items) {
		const Rect & r = item->BBox();
		nvgBeginPath(vg);
		nvgRect(vg, r.xf(), r.yf(), r.wf(), r.hf());
		nvgFillColor(vg, IsLeaf() ? nvgRGBA(0, 0, 255, 32) : nvgRGBA(255, 0, 0, 32));
		nvgFill(vg);

		nvgBeginPath(vg);
		nvgRect(vg, r.xf() + .5f, r.yf() + .5f, r.wf() - 1.f, r.hf() - 1.f);
		nvgStrokeColor(vg, IsLeaf() ? nvgRGBA(0, 0, 255, 32) : nvgRGBA(255, 0, 0, 32));
		nvgStroke(vg);
	}
}

void QuadTree::Split() {
	if (!IsLeaf() || m_divisions <= 0) {
		return;
	}

	float qw = m_hw / 2.f;
	float qh = m_hh / 2.f;
	m_branches[TopRight] = new QuadTree(m_cx + qw, m_cy - qh, qw, qh, m_divisions - 1);
	m_branches[TopLeft] = new QuadTree(m_cx - qw, m_cy - qh, qw, qh, m_divisions - 1);
	m_branches[BottomRight] = new QuadTree(m_cx + qw, m_cy + qh, qw, qh, m_divisions - 1);
	m_branches[BottomLeft] = new QuadTree(m_cx - qw, m_cy + qh, qw, qh, m_divisions - 1);
}

void QuadTree::Prune() {
	if (IsLeaf()) {
		return;
	}

	bool canPrune = true;
	for (int i = 0; i < _BranchCount; ++i) {
		canPrune = canPrune && m_branches[i]->IsLeaf() && m_branches[i]->m_items.empty();
	}

	if (canPrune) {
		for (int i = 0; i < _BranchCount; ++i) {
			delete m_branches[i];
			m_branches[i] = nullptr;
		}
	}
}

QuadTree::Branch QuadTree::FitInBranch(Item *item) {
	if (m_divisions <= 0 || !item) {
		return NoBranch;
	}

	const Rect & r = item->BBox();
	bool fullLeft = r.x + r.w <= m_cx;
	bool fullRight = r.x >= m_cx;
	bool fullTop = r.y + r.h <= m_cy;
	bool fullBottom = r.y >= m_cy;

	if (fullLeft && fullTop) {
		return TopLeft;
	}
	else if (fullLeft && fullBottom) {
		return BottomLeft;
	}
	else if (fullRight && fullTop) {
		return TopRight;
	}
	else if (fullRight && fullBottom) {
		return BottomRight;
	}
	else {
		return NoBranch;
	}
}
