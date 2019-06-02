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

#ifndef H_GOGH_NODEINPUTLISTMODEL
#define H_GOGH_NODEINPUTLISTMODEL

#include "AbstractSlotListModel.h"
#include "Graph.h"
#include "ParameterListModel.h"

namespace Gogh {
namespace Gui {

/**
* List of inputs attached to a node
*/
class NodeInputListModel : public AbstractSlotListModel
{
	Q_OBJECT
protected:
	// Implement AbstractSLotListModel virtual methods
	virtual std::vector<NodeInputPtr> & slotList();
	virtual const std::vector<NodeInputPtr> & slotList() const;
	virtual NodeInputPtr makeSlot() const;

public:
	enum Columns {
		NameColumn = 0,
		TypeColumn,
		_ColumnCount,
	};

private:
	NodePtr m_node;
};

} // namespace Gui
} // namespace Gogh

Q_DECLARE_METATYPE(std::shared_ptr<Gogh::Gui::NodeInputListModel>)

#endif // H_GOGH_NODEINPUTLISTMODEL
