#ifndef H_CODECNODE
#define H_CODECNODE

#include "Node.h"

class CodecNode : public Node
{
	Q_OBJECT

public:
	CodecNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const override;
	QVariant parmRawValue(int parm) const override;

	int parmMenuCount(int parm) const override;
	QString parmMenuLabel(int parm, int menu) const override;
	QVariant parmMenuValue(int parm, int menu) const override;

	bool setParm(int parm, QVariant value) override;

protected:
	std::string m_node_name;

private:
	int m_codec;
};

#endif // H_CODECNODE
