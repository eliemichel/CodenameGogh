#ifndef H_OUTPUTNODE
#define H_OUTPUTNODE

#include "Node.h"

class OutputNode : public Node
{
public:
	OutputNode();

	UiElement * createDelegate(UiLayout *popupLayout) override;

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

	void setFilenameUserDefined(bool userDefined) { m_isFilenameUserDefined = userDefined; }

protected:
	void inputSlotConnectEvent(SlotEvent *event) override;

private: // slots
	void onButtonClicked(int parm);

private:
	/// Used to keep track of whether the user set the filename or whether it has been auto generated and must hence be updated
	/// if filename is user-defined, it is displayed in bold
	bool m_isFilenameUserDefined;
};

#endif // H_OUTPUTNODE
