#ifndef H_MIXNODE
#define H_MIXNODE

#include "Node.h"

#include <vector>
#include <string>

class MixNode : public Node
{
	Q_OBJECT

public:
	MixNode();

public:
	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const;
	QVariant parmRawValue(int parm) const override;
	bool setParm(int parm, QVariant value) override;

	void read(QDataStream & stream) override;
	void write(QDataStream & stream) const override;

protected:
	void slotConnectEvent(SlotEvent *event) override;

protected:
	std::string m_node_name;

private:
	std::vector<std::string> m_streams;
};

#endif // H_MIXNODE
