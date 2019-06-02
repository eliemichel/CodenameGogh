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

#include <QComboBox>

#include "ParameterType.h"
#include "ParameterTypeDelegate.h"

using namespace Gogh::Gui;

QWidget* ParameterTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QComboBox *cb = new QComboBox(parent);

	using namespace Gogh::ParameterTypeUtils;
	for (int i = 0; i < _ParameterTypeCount; ++i)
	{
		cb->addItem(QString::fromStdString(parameterTypeName(fromInt(i))));
	}
	return cb;
}

void ParameterTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox *cb = qobject_cast<QComboBox*>(editor);
	Q_ASSERT(cb);
	cb->setCurrentIndex(index.data(Qt::EditRole).toInt());
}

void ParameterTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *cb = qobject_cast<QComboBox*>(editor);
	Q_ASSERT(cb);
	model->setData(index, cb->currentIndex(), Qt::EditRole);
}
