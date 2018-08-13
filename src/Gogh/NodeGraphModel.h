#ifndef H_NODEGRAPHMODEL
#define H_NODEGRAPHMODEL

#include "SlotIndex.h"
#include "Node.h"
#include "NodeType.h"

#include <QString> // should be replaced by a stream path class
#include <QAbstractItemModel>

#include <string>
#include <vector>
#include <set>

class EnvModel;
class Link;

class NodeGraphModel : public QAbstractItemModel
{
public:
	enum Column
	{
		TypeColumn,
		PosXColumn,
		PosYColumn,
		NameColumn,
		_ColumnCount, // special item, must remain the last one
	};

private:
	// Data blocks are blocks of data associated to nodes that have a variable size.
	// They are the direct children of the node indices
	enum IndexLevel
	{
		RootLevel,
		NodeLevel,
		BlockLevel,
		ElementLevel,
	};

	enum NodeBlocType
	{
		ParamBlock,
		InputSlotBlock,
		OutputSlotBlock,
		_BlockCount, // special item, must remain the last one
	};

	struct IndexData
	{
		IndexLevel level = RootLevel;
		int parentNodeIndex;
		int parentBlockIndex;
	};

	struct NodeEntry
	{
		Node *node;
		IndexData nodeIndex;
		IndexData blockIndex;
		IndexData elementIndex[_BlockCount];
	};

public:
	/**
	 * Utility function making the code much easier to reed
	 */
	static bool isRoot(QModelIndex index);

public:
	static const SlotIndex invalidSlot;
	static const std::set<SlotIndex> invalidSlotSet;

public:
	NodeGraphModel();
	~NodeGraphModel();

	// Accessors
	EnvModel *envModel() const { return m_envModel; }
	void setEnvModel(EnvModel *envModel) { m_envModel = envModel; }

	// I/O
	void LoadDefaultGraph();
	bool LoadGraph(QString filename);
	bool SaveGraph(QString filename);

	// Read node info
	int nodeCount() const { return static_cast<int>(m_nodeEntries.size()); }
	Node * node(int i) const { return m_nodeEntries[i]->node; }

	// Read link info
	bool canAddLink(const SlotIndex & origin, const SlotIndex & destination);
	const SlotIndex & originSlot(const SlotIndex & destination) const;
	const std::set<SlotIndex> & destinationSlots(const SlotIndex & origin) const;

	// Edit node info
	void addNode(Node *node);
	bool removeNode(Node *node);

	// Edit link info
	bool addLink(const SlotIndex & origin, const SlotIndex & destination);
	bool removeLink(const SlotIndex & destination);

	// Utility
	void broadcastNodeChange(int nodeIndex) { const QModelIndex & i = index(nodeIndex, 0); emit dataChanged(i, i); }

	/// Complexity: O(n)
	QModelIndex findByName(const std::string & name);

public: // overrides from QAbstractItemModel
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
	bool inParentBounds(const QModelIndex & index) const;
	IndexData *indexData(const QModelIndex & index) const;

private:
	std::vector<NodeEntry*> m_nodeEntries;

	/// Env Model is forwarded to nodes
	EnvModel *m_envModel;
};

#endif // H_NODEGRAPHMODEL
