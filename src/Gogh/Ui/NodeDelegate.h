#ifndef H_NODEDELEGATE
#define H_NODEDELEGATE

#include "Ui/UiBase.h"
#include "Signal.h"

class Node;

class NodeDelegate : public UiVBoxLayout {
public:
	NodeDelegate(Node *node, UiLayout *popupLayout = nullptr);

public: // signals
	Signal<int> buttonClicked; // (int param)

private: // slots
	void InsertParams(int first, int last);
	void RemoveParams(int first, int last);

private:
	Node *m_node;
	UiLayout *m_popupLayout;
};

#endif // H_NODEDELEGATE
