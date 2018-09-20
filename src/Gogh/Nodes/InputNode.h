#ifndef H_INPUTNODE
#define H_INPUTNODE

#include "Node.h"
#include "FileProbeProcess.h"

class InputNode : public Node
{
public:
	InputNode();

	UiElement * createDelegate(UiLayout *popupLayout) override;

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

private: // slots
	/// Update output slots, corresponding to input file streams
	void updateStreams();
	void onButtonClicked(int parm);

private:
	std::string m_filename;
	FileProbeProcess m_probeProcess;
};

#endif // H_INPUTNODE
