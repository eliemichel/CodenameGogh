#ifndef H_NODEWIDGET
#define H_NODEWIDGET

#include "Slot.h"

#include <QWidget>
#include <QVariant>

#include <vector>
#include <string>

class LinkGraphicsItem;

/**
 * This structure is transmitted among the graph nodes while building the
 * render command. Feel free to add any field required to properly build
 * the command.
 */
struct RenderCommand {
	// raw command being built
	std::string cmd;

	// error message that may be filled when returning false in buildRenderCommand
	std::string err;
};

class NodeWidget : public QWidget
{
	Q_OBJECT;

public:
	explicit NodeWidget(QWidget *parent = 0);
	~NodeWidget();

	const std::vector<Slot*> inputSlots() const { return m_inputSlots; }
	Slot* newInputSlot();

	const std::vector<Slot*> outputSlots() const { return m_outputSlots; }
	Slot* newOutputSlot();

	/**
	 * Function that contains the logic of the node. This must be reimplemented
	 * in each node and is called when building the render command.
	 */
	virtual bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const { return true; }

	/**
	 * Try to call buildRenderCommand by providing a pointer to a slot instead
	 * of the slot index. This iterates through the inputs until finding one
	 * that equals.
	 */
	bool buildRenderCommand(const Slot *slot, RenderCommand  & cmd) const;

	/**
	 * Convenience function calling buildRenderCommand() on the source output slot
	 * connected to the input at index inputIndex.
	 */
	bool parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const;

	/**
	 * I/O function, used to save and load scenes.
	 * Reimplement this in subclasses to symmetrically write and read back raw node data.
	 */
	virtual void read(QDataStream & stream);
	virtual void write(QDataStream & stream) const;

public: // data model
	virtual int parmCount() const { return 0; }
	virtual QString parmName(int parm) const { return QString(); }
	virtual QVariant parmEval(int parm) const { return QVariant(); }
	virtual void setParm(int parm, QVariant value) {}

private:
	std::vector<Slot*> m_inputSlots;
	std::vector<Slot*> m_outputSlots;
};

#endif // H_NODEWIDGET