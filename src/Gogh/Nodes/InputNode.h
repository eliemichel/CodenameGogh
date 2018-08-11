#ifndef H_INPUTNODE
#define H_INPUTNODE

#include "Node.h"

class InputNode : public Node
{
	Q_OBJECT

public:
	InputNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const override;
	QVariant parmRawValue(int parm) const override;
	bool setParm(int parm, QVariant value) override;

protected:
	std::string m_node_name;

private:
	std::string m_filename;
};

#endif // H_INPUTNODE
