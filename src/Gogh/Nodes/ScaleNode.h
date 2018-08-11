#ifndef H_SCALENODE
#define H_SCALENODE

#include "Node.h"

class ScaleNode : public Node
{
	Q_OBJECT

public:
	ScaleNode();

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
	int m_width;
	int m_height;
};

#endif // H_SCALENODE
