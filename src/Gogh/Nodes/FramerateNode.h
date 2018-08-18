#ifndef H_FRAMERATENODE
#define H_FRAMERATENODE

#include "Node.h"

class FramerateNode : public Node
{
	Q_OBJECT

public:
	FramerateNode();

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const override;
	QVariant parmRawValue(int parm) const override;
	bool setParm(int parm, QVariant value) override;

private:
	int m_framerate;
};

#endif // H_FRAMERATENODE
