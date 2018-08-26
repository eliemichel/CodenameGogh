#ifndef H_OUTPUTNODE
#define H_OUTPUTNODE

#include "Node.h"

class OutputNode : public Node
{
	Q_OBJECT

public:
	OutputNode();

	QWidget *createEditor(QWidget *parent = nullptr) override;

	bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const override;

	void setFilenameUserDefined(bool userDefined) { m_isFilenameUserDefined = userDefined; }

public: // data model
	int parmCount() const override;
	QString parmName(int parm) const override;
	ParmType parmType(int parm) const override;
	QVariant parmRawValue(int parm) const override;
	bool setParm(int parm, QVariant value) override;

protected:
	void slotConnectEvent(SlotEvent *event) override;

private:
	std::string m_filename;
	bool m_weboptimized;

	/// Used to keep track of whether the user set the filename or whether it has been auto generated and must hence be updated
	/// if filename is user-defined, it is displayed in bold
	bool m_isFilenameUserDefined;
};

#endif // H_OUTPUTNODE
