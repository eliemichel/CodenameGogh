#ifndef H_ENVMODEL
#define H_ENVMODEL

#include <QAbstractItemModel>

#include <string>
#include <map>
#include <vector>

class EnvModel : public QAbstractItemModel
{
public:
	enum Column {
		VarColumn,
		ValueColumn,
	};

public:
	EnvModel();

	std::string & at(const std::string & key) { return m_env.at(key); }
	const std::string & at(const std::string & key) const { return m_env.at(key); }
	std::map<std::string, std::string> & env() { return m_env; }
	const std::map<std::string, std::string> & env() const { return m_env; }

public: // overrides from QAbstractItemModel
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
	static bool isRoot(const QModelIndex & index) { return !index.isValid(); }
	bool isEntryIndex(const QModelIndex & index) const;

private:
	std::map<std::string, std::string> m_env;
	std::vector<std::string> m_orderedKeys;
};

#endif // H_ENVMODEL
