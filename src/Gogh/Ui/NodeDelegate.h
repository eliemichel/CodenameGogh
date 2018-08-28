#ifndef H_NODEDELEGATE
#define H_NODEDELEGATE

#include "Ui/UiBase.h"

class Node;

class NodeDelegate : public UiVBoxLayout {
public:
	NodeDelegate(Node *node, UiLayout *popupLayout = nullptr);

private:
	Node *m_node;
};

#endif // H_NODEDELEGATE
