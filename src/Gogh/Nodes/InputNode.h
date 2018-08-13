#ifndef H_INPUTNODE
#define H_INPUTNODE

#include "Node.h"
#include "FileProbeProcess.h"

class InputNode : public Node
{
	Q_OBJECT

public:
	InputNode();

	QWidget * createEditor(QWidget *parent = nullptr) override;

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const override;
	QVariant parmRawValue(int parm) const override;
	bool setParm(int parm, QVariant value) override;

private slots:
	/// Update output slots, corresponding to input file streams
	void updateStreams();
	void onButtonClicked(int parm);

private:
	std::string m_filename;
	FileProbeProcess m_probeProcess;
};

#endif // H_INPUTNODE
