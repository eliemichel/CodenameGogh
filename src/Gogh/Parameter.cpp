/**
* This file is part of Gogh.
* It is released under the terms of the MIT License:
*
* Copyright (c) 2018 - 2019 -- Élie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided “as is”, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#include "Parameter.h"
#include "Logger.h"

using namespace Gogh;

Parameter::Parameter()
	: m_type(NoneType)
{}

Parameter::~Parameter() {
	destroyed.fire();
}

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
		nameChanged.fire();
	}
}

void Parameter::setType(ParameterType type)
{
	if (m_type != type) {
		m_type = type;
		typeChanged.fire();
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
	menuLabelChanged.fire(item);
}

void Parameter::setMenuValue(int item, const Variant & value)
{
	if (item < 0 || item >= menuCount())
	{
		WARN_LOG << "Invalid menu item: " << item << " (size is " << menuCount() << ")";
		return;
	}

	m_menu[item].value = value;
	menuValueChanged.fire(item);
}

void Parameter::insertMenuItems(int first, int last)
{
	if (first < 0 || first > menuCount() || last < first)
	{
		WARN_LOG << "Invalid menu insert: " << first << " to " << last << " (current size is " << menuCount() << ")";
		return;
	}

	aboutToInsertMenuItems.fire(first, last);
	m_menu.insert(m_menu.begin() + first, last - first + 1, MenuItem());
}

void Parameter::removeMenuItems(int first, int last)
{
	if (first < 0 || last >= menuCount() || last < first)
	{
		WARN_LOG << "Invalid menu remove: " << first << " to " << last << " (size is " << menuCount() << ")";
		return;
	}

	aboutToRemoveMenuItems.fire(first, last);
	m_menu.erase(m_menu.begin() + first, m_menu.begin() + last + 1);
}

std::string Parameter::evalAsString() const {
	std::string value;
	switch (type())
	{
	case EnumType:
	{
		int menu = rawValue().toInt();
		value = menuValue(menu).toString();
		break;
	}
	default:
		value = rawValue().toString();
	}

	// TODO: Environment variables
	/*
	if (EnvModel *env = envModel()) {
		for (auto it = env->env().cbegin(); it != env->env().cend(); ++it)
		{
			value = value.replace(QString::fromStdString("$" + it->first), QString::fromStdString(it->second));
		}
	}
	*/
	return value;
}

int Parameter::evalAsInt() const {
	switch (type())
	{
	case EnumType:
	{
		int menu = rawValue().toInt();
		return menuValue(menu).toInt();
		break;
	}
	default:
		return rawValue().toInt();
	}
}

int Parameter::evalAsBool() const {
	switch (type())
	{
	case EnumType:
	{
		int menu = rawValue().toBool();
		return menuValue(menu).toBool();
		break;
	}
	default:
		return rawValue().toBool();
	}
}
