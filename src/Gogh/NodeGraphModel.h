#ifndef H_NODEGRAPHMODEL
#define H_NODEGRAPHMODEL

#include "SlotIndex.h"
#include "Node.h"

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
	// When adding a new type here, handle it in the body of buildNode() and nodeTypeToString() as well
	enum NodeType
	{
		NODE_OUTPUT = 0,
		NODE_INPUT  = 1,
		NODE_SCALE  = 2,
		NODE_CODEC  = 3,
		NODE_MIX    = 4,
	};

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
	// Factory function building a new node from a given type.
	// This is the only place holding a mapping from type enum to type classes
	// and must be updated any time a new node type is defined
	static Node * buildNode(int type);

	/**
	 * Convert node type to a string, for display purpose only
	 */
	static std::string nodeTypeToString(int type);

	/**
	 * Utility function making the code much easier to reed
	 */
	static bool isRoot(QModelIndex index);

public:
	static SlotIndex invalidSlot;
	static std::set<SlotIndex> invalidSlotSet;

public:
	NodeGraphModel();
	~NodeGraphModel();

	EnvModel *envModel() const { return m_envModel; }
	void setEnvModel(EnvModel *envModel) { m_envModel = envModel; }

public: // overrides from QAbstractItemModel
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

public:
	void LoadDefaultGraph();
	bool LoadGraph(QString filename);
	bool SaveGraph(QString filename);

	void addNode(Node *node);
	// TODO: removeNode() (to be called in dtor)
	const std::vector<NodeEntry*> & nodes() const { return m_nodeEntries; }

	Node * node(int i) const { return m_nodeEntries[i]->node; }

	bool canAddLink(const SlotIndex & origin, const SlotIndex & destination);
	bool addLink(const SlotIndex & origin, const SlotIndex & destination);
	bool removeLink(const SlotIndex & destination);

	const SlotIndex & originSlot(const SlotIndex & destination) const;
	const std::set<SlotIndex> & destinationSlots(const SlotIndex & origin) const;

	void broadcastNodeChange(const QModelIndex & nodeIndex) { emit dataChanged(nodeIndex, nodeIndex); }

private:
	bool inParentBounds(const QModelIndex & index) const;
	IndexData *indexData(const QModelIndex & index) const;

private:
	std::vector<NodeEntry*> m_nodeEntries;

	/// Env Model is forwarded to nodes
	EnvModel *m_envModel;
};

#endif // H_NODEGRAPHMODEL
