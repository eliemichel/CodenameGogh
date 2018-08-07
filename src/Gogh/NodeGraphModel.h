#ifndef H_NODEGRAPHMODEL
#define H_NODEGRAPHMODEL

#include "NodeWidget.h"

#include <QString> // should be replaced by a stream path class
#include <QAbstractItemModel>

#include <string>
#include <vector>

class EnvModel;

class NodeGraphModel : public QAbstractItemModel
{
public:
	// This is a temprary structure, while we don't have a proper Node type
	struct NodeEntry
	{
		NodeWidget *node;
		int type;
		float x, y;
		std::string name;
	};

	// When adding a new type here, handle it in the body of buildNode() and nodeTypeToString() as well
	enum NodeType
	{
		NODE_OUTPUT = 0,
		NODE_INPUT  = 1,
		NODE_SCALE  = 2,
		NODE_CODEC  = 3,
	};

	enum Column
	{
		TypeColumn,
		PosXColumn,
		PosYColumn,
		NameColumn,
		_ColumnCount, // special item, must remain the last one
	};

public:
	// Factory function building a new node from a given type.
	// This is the only place holding a mapping from type enum to type classes
	// and must be updated any time a new node type is defined
	static NodeWidget * buildNode(int type);

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

	void addNode(NodeWidget *node, int type, float x, float y, std::string name);
	const std::vector<NodeEntry> & nodes() const { return m_nodes; }

private:
	bool inParentBounds(const QModelIndex & index) const;
	bool isNodeIndex(const QModelIndex & index) const;

private:
	/**
	 * This should be a vector of non GUI objects, but for now we don't have a
	 * node widget generator from node definition, and I want to keep it simple
	 * to add new node type.
	 */
	std::vector<NodeEntry> m_nodes;
	EnvModel *m_envModel;
};

#endif // H_NODEGRAPHMODEL
