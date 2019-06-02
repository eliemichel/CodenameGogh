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

#ifndef H_GOGH_PARAMETERLISTVIEW
#define H_GOGH_PARAMETERLISTVIEW

#include <QTreeView>
#include <QModelIndex>

class QTreeView;
class QMenu;


namespace Gogh {
namespace Gui {

/**
 * Variant of QTreeView to display a ParameterListModel. This view is mainly
 * used for debugging and advanced operations. Regular users will use the
 * ParameterView instead (TODO: find more distinct names)
 * Try to depend as little as possible on ParameterListModel and instead use
 * the regular QAbstractItemModel methods
 */
class ParameterListView : public QTreeView
{
	Q_OBJECT
public:
	ParameterListView(QWidget *parent = nullptr);

private slots:
	void onContextMenu(const QPoint & point);
	void removeParameter();
	void insertParameterAbove();
	void insertParameterBellow();

private:
	QMenu *m_contextMenu;
	/// cache the index of the item on which the context menu is open
	QModelIndex m_contextMenuParameterIndex;
};

} // namespace Gui
} // namespace Gogh

#endif // H_GOGH_PARAMETERLISTVIEW
