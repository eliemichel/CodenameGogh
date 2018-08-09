#ifndef H_NODEGRAPHMODEL
#define H_NODEGRAPHMODEL

#include "NodeWidget.h"

#include <QString> // should be replaced by a stream path class
#include <QAbstractItemModel>

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class EnvModel;
class Link;
typedef NodeWidget Node;

struct SlotIndex
{
	SlotIndex() : node(-1) {}
	SlotIndex(int _node, int _slot) : node(_node), slot(_slot) {}

	int node;
	int slot;

	bool isConnected() const { return node != -1; }

	bool operator<(const SlotIndex &other) const { return node < other.node || slot < other.slot; }
};

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
		int type;
		float x, y;
		std::string name;
		std::vector<SlotIndex> inputLinks;
		std::vector<std::set<SlotIndex>> outputLinks; // /!\ Problem: there can be several output links per output slot

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
	NodeGraphModel();

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

	void addNode(Node *node, int type, float x, float y, std::string name);
	const std::vector<NodeEntry> & nodes() const { return m_nodes; }
	Node * nodeData(int i) { return m_nodes[i].node; }

	bool canAddLink(int originNode, int originSlot, int destinationNode, int destinationSlot);
	bool addLink(int originNode, int originSlot, int destinationNode, int destinationSlot);
	const SlotIndex & sourceSlot(int destinationNode, int destinationSlot) const;
	const std::set<SlotIndex> & destinationSlots(int sourceNode, int sourceSlot) const;

	// TODO: remove me
	int nodeIndex(const Node *node) { return m_nodeLUT.at(node); }

private:
	bool inParentBounds(const QModelIndex & index) const;
	IndexData *indexData(const QModelIndex & index) const;

private:
	/**
	 * This should be a list of non GUI objects, but for now we don't have a
	 * node widget generator from node definition, and I want to keep it simple
	 * to add new node type.
	 */
	mutable std::vector<NodeEntry> m_nodes;
	EnvModel *m_envModel;
	std::unordered_map<const Node*, int> m_nodeLUT;
};

#endif // H_NODEGRAPHMODEL
