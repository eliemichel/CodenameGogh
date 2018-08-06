#include "EnvModel.h"

#include <QModelIndex>

EnvModel::EnvModel()
{
	m_env["INPUT"] = "";
	m_env["OUTPUT"] = "";
	m_env["FFMPEG"] = "E:/Program Files/ffmpeg/bin/ffmpeg";

	m_orderedKeys.push_back("INPUT");
	m_orderedKeys.push_back("OUTPUT");
	m_orderedKeys.push_back("FFMPEG");
}

std::string EnvModel::eval(std::string var) const
{
	std::map<std::string, std::string>::const_iterator it = m_env.find(var);
	if (it != m_env.end())
	{
		return it->second;
	}
	else
	{
		return "";
	}
}

QModelIndex EnvModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!isRoot(parent))
	{
		return QModelIndex();
	}
	else if (row < 0 || row >= rowCount() || column < 0 || column >= columnCount())
	{
		return QModelIndex();
	}
	else
	{
		return createIndex(row, column, nullptr);
	}
}

QModelIndex EnvModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int EnvModel::rowCount(const QModelIndex & parent) const
{
	return isRoot(parent) ? static_cast<int>(m_orderedKeys.size()) : 0;
}

int EnvModel::columnCount(const QModelIndex & parent) const
{
	return isRoot(parent) ? 2 : 0;
}

QVariant EnvModel::data(const QModelIndex & index, int role) const
{
	if (isEntryIndex(index) && (role == Qt::DisplayRole || role == Qt::EditRole))
	{
		switch (index.column())
		{
		case VarColumn:
			return QString::fromStdString(m_orderedKeys[index.row()]);
		case ValueColumn:
			return QString::fromStdString(m_env.at(m_orderedKeys[index.row()]));
		default:
			return QVariant();
		}
	}
	else
	{
		return QVariant();
	}
}

QVariant EnvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		switch (section)
		{
		case VarColumn:
			return "Variable";
		case ValueColumn:
			return "Value";
		default:
			return QVariant();
		}
	}
	else
	{
		return QVariant();
	}
}

bool EnvModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (isEntryIndex(index) && index.column() == ValueColumn)
	{
		m_env.at(m_orderedKeys[index.row()]) = value.toString().toStdString();
		return true;
	}
	else
	{
		return false;
	}
}

Qt::ItemFlags EnvModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags f = QAbstractItemModel::flags(index);
	if (isEntryIndex(index) && index.column() == ValueColumn)
	{
		return f | Qt::ItemIsEditable;
	}
	else
	{
		return f;
	}
}

bool EnvModel::isEntryIndex(const QModelIndex & index) const
{
	const QModelIndex & parent = index.parent();
	return index.isValid() && isRoot(parent) && index.row() >= 0 && index.row() < rowCount() && index.column() >= 0 && index.column() < columnCount();
}
