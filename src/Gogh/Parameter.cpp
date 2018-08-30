#include "Parameter.h"
#include "Logger.h"

Parameter::Parameter(QObject *parent)
	: QObject(parent)
	, m_type(NoneType)
{}

int Parameter::menuCount() const
{
	return static_cast<int>(m_menu.size());
}

std::string Parameter::menuLabel(int item) const
{
	return item < 0 || item >= menuCount() ? std::string() : m_menu[item].label;
}

Variant Parameter::menuValue(int item) const
{
	return item < 0 || item >= menuCount() ? Variant() : m_menu[item].value;
}

bool Parameter::set(const Variant & value)
{
	if (m_rawValue != value) {
		m_rawValue = value;
		valueChanged.fire();
	}
	return true;
}

void Parameter::setName(const std::string & name)
{
	if (m_name != name) {
		m_name = name;
		emit nameChanged();
	}
}

void Parameter::setType(ParmType type)
{
	if (m_type != type) {
		m_type = type;
		emit typeChanged();
	}
}

void Parameter::setMenuLabel(int item, const std::string & label)
{
	if (item < 0 || item >= menuCount())
	{
		WARN_LOG << "Invalid menu item: " << item;
		return;
	}

	m_menu[item].label = label;
	emit menuLabelChanged(item);
}

void Parameter::setMenuValue(int item, const Variant & value)
{
	if (item < 0 || item >= menuCount())
	{
		WARN_LOG << "Invalid menu item: " << item << " (size is " << menuCount() << ")";
		return;
	}

	m_menu[item].value = value;
	emit menuValueChanged(item);
}

void Parameter::insertMenuItems(int first, int last)
{
	if (first < 0 || first > menuCount() || last < first)
	{
		WARN_LOG << "Invalid menu insert: " << first << " to " << last << " (current size is " << menuCount() << ")";
		return;
	}

	emit aboutToInsertMenuItems(first, last);
	m_menu.insert(m_menu.begin() + first, last - first + 1, MenuItem());
}

void Parameter::removeMenuItems(int first, int last)
{
	if (first < 0 || last >= menuCount() || last < first)
	{
		WARN_LOG << "Invalid menu remove: " << first << " to " << last << " (size is " << menuCount() << ")";
		return;
	}

	emit aboutToRemoveMenuItems(first, last);
	m_menu.erase(m_menu.begin() + first, m_menu.begin() + last + 1);
}
