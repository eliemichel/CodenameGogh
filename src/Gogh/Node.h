#ifndef H_NODE
#define H_NODE

#include "ParameterType.h"
#include "RenderCommand.h"
#include "utils/FileStream.h"
#include "Signal.h"

#include <set>
#include <vector>
#include <string>

class UiElement;
class UiLayout;

class Parameter;
class InputSlot;
class OutputSlot;

class Node
{
public:
	class SlotEvent
	{
		friend class Node;

	public:
		/// index in inputSlots() or outputSlots() of the slot that has been connected
		int slotIndex() const { return m_slotIndex; };

	private:
		SlotEvent(int slotIndex)
			: m_slotIndex(slotIndex)
		{}

	private:
		int m_slotIndex;
	};

public:
	Node();
	Node(const Node &) = delete; // signal emitters must not be copied
	~Node();

	// // Getters // //

	// TODO: make some protected

	/**
	 * The node type is a number representing its derived class
	 */
	int type() const { return m_type; }

	/**
	 * The display name of the node
	 */
	const std::string & name() const { return m_name; }

	/**
	 * Return the number of parameters in the node
	 */
	int paramCount() const;

	/** 
	 * Get parameter at index i. Use the implementation returning a non const
	 * reference to update the parameter.
	 */
	const Parameter & param(int i) const;
	Parameter & param(int i);

	/**
	 * Return the number of input slots in the node
	 */
	int inputSlotCount() const;

	/**
	 * Get input slot at index i.
	 */
	const InputSlot & inputSlot(int i) const;
	InputSlot & inputSlot(int i);

	/**
	 * Return the number of output slots in the node
	 */
	int outputSlotCount() const;

	/**
	 * Get output slot at index i.
	 */
	const OutputSlot & outputSlot(int i) const;
	OutputSlot & outputSlot(int i);

	// // Setters // //

	// TODO: Type should not be editable
	void setType(int type) { m_type = type; }

	void setName(const std::string & name) { m_name = name; }

	/**
	 * Insert new parameters. The inserted params will have indexes from <first>
	 * to <last> and the next params will be shifted by last - first + 1.
	 * The emits the signal aboutToInsertParams before changing the underlying data
	 */
	void insertParams(int first, int last);

	/**
	 * Remove parameters from <first> to <last> included.
	 * The emits the signal aboutToRemoveParams before changing the underlying data
	 */
	void removeParams(int first, int last);

	/**
	 * Insert new input slots. The inserted slots will have indexes from <first>
	 * to <last> and the next slots will be shifted by last - first + 1.
	 * The emits the signal aboutToInsertInputSlots before changing the underlying data
	 */
	void insertInputSlots(int first, int last);

	/**
	 * Remove input slots from <first> to <last> included.
	 * The emits the signal aboutToRemoveInputSlots before changing the underlying data
	 */
	void removeInputSlots(int first, int last);

	/**
	* Insert new output slots. The inserted slots will have indexes from <first>
	* to <last> and the next slots will be shifted by last - first + 1.
	* The emits the signal aboutToInsertOutputSlots before changing the underlying data
	*/
	void insertOutputSlots(int first, int last);

	/**
	* Remove output slots from <first> to <last> included.
	* The emits the signal aboutToRemoveOutputSlots before changing the underlying data
	*/
	void removeOutputSlots(int first, int last);

	// Extra utility inserters

	/**
	 * Insert new parameters at the end.
	 * This is a short for insertParams(paramCount(), paramCount() + n - 1).
	 */
	void appendParams(int n) { insertParams(paramCount(), paramCount() + n - 1); }
	void appendParam() { appendParams(1); }

	/**
	 * Insert new input slots at the end.
	 * This is a short for insertInputSlots(inputSlotCount(), inputSlotCount() + n - 1).
	 */
	void appendInputSlots(int n) { insertInputSlots(inputSlotCount(), inputSlotCount() + n - 1); }
	void appendInputSlot() { appendInputSlots(1); }

	/**
	 * Insert new output slots at the end.
	 * This is a short for insertOutputSlots(outputSlotCount(), outputSlotCount() + n - 1).
	 */
	void appendOutputSlots(int n) { insertOutputSlots(outputSlotCount(), outputSlotCount() + n - 1); }
	void appendOutputSlot() { appendOutputSlots(1); }
	void removeOutputSlots() { removeOutputSlots(0, outputSlotCount() - 1); }


public:
	/**
	 * Create an editor widget to operate on this node's parms.
	 * This is the only method that handle graphical objects.
	 * [Elie] Why do we need this? It is likely to be removed if there is no good reason to keep it
	 */
	virtual UiElement * createDelegate(UiLayout *popupLayout = nullptr);

public: // signals
	/// emitted after inserting new parameters from position <first> to <last> included
	Signal<int, int> insertedParams; // (int first, int last)

	/// emitted before removing parameters from position <first> to <last> included
	Signal<int, int> aboutToRemoveParams; // (int first, int last)

	/// emitted after  inserting new input slots from position <first> to <last> included
	Signal<int, int> insertedInputSlots; // (int first, int last)

	/// emitted before removing input slots from position <first> to <last> included
	Signal<int, int> aboutToRemoveInputSlots; // (int first, int last)

	/// emitted after  inserting new output slots from position <first> to <last> included
	Signal<int, int> insertedOutputSlots; // (int first, int last)

	/// emitted before removing output slots from position <first> to <last> included
	Signal<int, int> aboutToRemoveOutputSlots; // (int first, int last)

	/// Signal emitted just after destructing the object
	Signal<> destroyed;

public:

	/**
	 * Function that contains the logic of the node. This must be reimplemented
	 * in each node and is called when building the render command.
	 */
	virtual bool buildRenderCommand(int outputIndex, RenderCommand & cmd) const { return true; }
	bool buildRenderCommand(OutputSlot *slot, RenderCommand & cmd) const;

	/**
	 * Convenience function calling buildRenderCommand() on the source output slot
	 * connected to the input at index inputIndex.
	 */
	bool parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const;

	virtual void read(InputStream & stream);
	virtual void write(OutputFileStream & stream) const;

protected:
	/// event fired after connecting a link to an input slot
	virtual void inputSlotConnectEvent(SlotEvent *event) {}
	/// event fired after disconnecting a link from an input slot
	virtual void inputSlotDisconnectEvent(SlotEvent *event) {}
	/// event fired after connecting a link to an output slot
	virtual void outputSlotConnectEvent(SlotEvent *event) {}
	/// event fired after disconnecting a link from an output slot
	virtual void outputSlotDisconnectEvent(SlotEvent *event) {}

private:
	/**
	 * This is used in graphics view to initiate slotConnectEvent() propagation.
	 * This should ultimately be modified so do not use this method anywhere else
	 * Reserved to NodeGraphModel
	 */
	void fireInputSlotConnectEvent(InputSlot *slot);
	void fireOutputSlotConnectEvent(OutputSlot *slot);

	// TODO: fire inputSlotDisconnectEvent and outputSlotDisconnectEvent

private:
	int m_type;
	float m_x, m_y;
	std::string m_name;

	std::vector<Parameter*> m_params;
	std::vector<InputSlot*> m_inputSlots;
	std::vector<OutputSlot*> m_outputSlots;
};

#endif // H_NODE
