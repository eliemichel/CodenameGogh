#ifndef H_NODEWIDGET
#define H_NODEWIDGET

#include "Slot.h"
#include "Logger.h"

#include <QWidget>
#include <QVariant>
#include <QModelIndex>

#include <vector>
#include <string>

class LinkGraphicsItem;
class EnvModel;
class NodeGraphModel;

/**
 * This structure is transmitted among the graph nodes while building the
 * render command. Feel free to add any field required to properly build
 * the command.
 */
struct RenderCommand {
	// raw command being built
	std::vector<std::string> cmd;

	// error message that may be filled when returning false in buildRenderCommand
	std::string err;
};

class NodeWidget : public QWidget
{
	Q_OBJECT;

public:
	class SlotEvent
	{
		friend class NodeWidget;

	public:
		/// index in inputSlots() or outputSlots() of the slot that has been connected
		int slotIndex() const { return m_slotIndex; };
		/// true if the connected slot is an input slot, false if it is an output
		bool isInputSlot() const { return m_isInputSlot; };

	private:
		SlotEvent(int slotIndex, bool isInputSlot)
			: m_slotIndex(slotIndex), m_isInputSlot(isInputSlot)
		{}

	private:
		int m_slotIndex;
		bool m_isInputSlot;
	};

public:
	explicit NodeWidget(QWidget *parent = 0);
	~NodeWidget();

	int inputSlotsCount() const { return static_cast<int>(m_inputSlots.size()); }
	const std::vector<Slot*> inputSlots() const { return m_inputSlots; }
	Slot* newInputSlot();

	int outputSlotsCount() const { return static_cast<int>(m_outputSlots.size()); }
	const std::vector<Slot*> outputSlots() const { return m_outputSlots; }
	Slot* newOutputSlot();

	EnvModel *envModel() const { return m_envModel; }
	void setEnvModel(EnvModel *envModel) { m_envModel = envModel; }

	const QModelIndex & modelIndex() const { return m_modelIndex; }
	void setModelIndex(const QModelIndex & index) { m_modelIndex = index; }

	NodeGraphModel *graphModel() const { return m_graphModel; }
	void setGraphModel(NodeGraphModel *model) { m_graphModel = model; }

	int outputSlotIndex(const Slot *slot) const;
	int inputSlotIndex(const Slot *slot) const;

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
	 * The default implementation relies on parmCount, parmEval and setParm.
	 * Reimplement this in subclasses to symmetrically write and read back raw
	 * node data in a custom way.
	 */
	virtual void read(QDataStream & stream);
	virtual void write(QDataStream & stream) const;

	/**
	 * This is used in graphics view to initiate slotConnectEvent() propagation.
	 * This should ultimately be modified so do not use this method anywhere else
	 * Reserved to NodeGraphModel
	 */
	void fireSlotConnectEvent(Slot *slot, bool isInput);
	void fireSlotDisconnectEvent(Slot *slot, bool isInput);
	void fireSlotConnectEvent(int slotIndex, bool isInput);
	void fireSlotDisconnectEvent(int slotIndex, bool isInput);

public: // data model
	virtual int parmCount() const { return 0; }
	virtual QString parmName(int parm) const { return QString(); }
	virtual QVariant parmEval(int parm) const { return QVariant(); }
	virtual void setParm(int parm, QVariant value) {}

	/// This should be called parmEval and the current parmEval should be parmRawValue but I don't want to break the API yet
	QString parmFullEval(int parm) const;

protected:
	virtual void slotConnectEvent(SlotEvent *event) {}
	virtual void slotDisconnectEvent(SlotEvent *event) {}

private:
	std::vector<Slot*> m_inputSlots;
	std::vector<Slot*> m_outputSlots;
	EnvModel *m_envModel;
	NodeGraphModel *m_graphModel;
	QModelIndex m_modelIndex;
};

#endif // H_NODEWIDGET
