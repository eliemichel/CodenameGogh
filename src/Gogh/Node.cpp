#include "Node.h"
#include "Ui/UiBase.h"
#include "Ui/NodeDelegate.h"
#include "Parameter.h"
#include "Slot.h"
#include "Link.h"
#include "Logger.h"

#include <cassert>

Node::Node()
{}

Node::~Node() {
	removeParams(0, paramCount() - 1);
	removeInputSlots(0, inputSlotCount() - 1);
	removeOutputSlots(0, outputSlotCount() - 1);
	destroyed.fire();
}

// // Getters // //

int Node::paramCount() const {
	return static_cast<int>(m_params.size());
}

const Parameter & Node::param(int i) const {
	if (i < 0 || i >= paramCount()) {
		ERR_LOG << "Trying to access an invalid parameter index: #" << i << " (not in range [0," << paramCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), paramCount() - 1);
	}
	return *m_params.at(i);
}

Parameter & Node::param(int i) {
	if (i < 0 || i >= paramCount()) {
		ERR_LOG << "Trying to access an invalid parameter index: #" << i << " (not in range [0," << paramCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), paramCount() - 1);
	}
	return *m_params.at(i);
}

int Node::inputSlotCount() const {
	return static_cast<int>(m_inputSlots.size());
}

const InputSlot & Node::inputSlot(int i) const {
	if (i < 0 || i >= inputSlotCount()) {
		ERR_LOG << "Trying to access an invalid input slot index: #" << i << " (not in range [0," << inputSlotCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), inputSlotCount() - 1);
	}
	return *m_inputSlots.at(i);
}

InputSlot & Node::inputSlot(int i) {
	if (i < 0 || i >= inputSlotCount()) {
		ERR_LOG << "Trying to access an invalid input slot index: #" << i << " (not in range [0," << inputSlotCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), inputSlotCount() - 1);
	}
	return *m_inputSlots.at(i);
}

int Node::outputSlotCount() const {
	return static_cast<int>(m_outputSlots.size());
}

const OutputSlot & Node::outputSlot(int i) const {
	if (i < 0 || i >= outputSlotCount()) {
		ERR_LOG << "Trying to access an invalid output slot index: #" << i << " (not in range [0," << outputSlotCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), outputSlotCount() - 1);
	}
	return *m_outputSlots.at(i);
}

OutputSlot & Node::outputSlot(int i) {
	if (i < 0 || i >= outputSlotCount()) {
		ERR_LOG << "Trying to access an invalid output slot index: #" << i << " (not in range [0," << outputSlotCount() << "])";
		assert(false);
		i = std::min(std::max(0, i), outputSlotCount() - 1);
	}
	return *m_outputSlots.at(i);
}

// // Setters // //

void Node::insertParams(int first, int last)
{
	if (first < 0 || first > paramCount() || last < first)
	{
		WARN_LOG << "Invalid parameter insert: " << first << " to " << last << " (current size is " << paramCount() << ")";
		return;
	}

	m_params.insert(m_params.begin() + first, last - first + 1, nullptr);
	for (auto it = m_params.begin() + first; it != m_params.begin() + last + 1; ++it) {
		*it = new Parameter();
	}
	insertedParams.fire(first, last);
}

void Node::removeParams(int first, int last)
{
	if (first < 0 || last >= paramCount() || last < first)
	{
		WARN_LOG << "Invalid parameter remove: " << first << " to " << last << " (size is " << paramCount() << ")";
		return;
	}

	aboutToRemoveParams.fire(first, last);
	for (auto it = m_params.begin() + first; it != m_params.begin() + last + 1; ++it) {
		delete *it;
	}
	m_params.erase(m_params.begin() + first, m_params.begin() + last + 1);
}

void Node::insertInputSlots(int first, int last)
{
	if (first < 0 || first > inputSlotCount() || last < first)
	{
		WARN_LOG << "Invalid input slot insert: " << first << " to " << last << " (current size is " << inputSlotCount() << ")";
		return;
	}

	m_inputSlots.insert(m_inputSlots.begin() + first, last - first + 1, nullptr);
	for (auto it = m_inputSlots.begin() + first; it != m_inputSlots.begin() + last + 1; ++it) {
		InputSlot *slot = new InputSlot(this);
		slot->linkAdded.connect([this, slot]() { fireInputSlotConnectEvent(slot); });
		*it = slot;
	}
	insertedInputSlots.fire(first, last);
}

void Node::removeInputSlots(int first, int last)
{
	if (first < 0 || last >= inputSlotCount() || last < first)
	{
		WARN_LOG << "Invalid input slot remove: " << first << " to " << last << " (size is " << inputSlotCount() << ")";
		return;
	}

	aboutToRemoveInputSlots.fire(first, last);
	for (auto it = m_inputSlots.begin() + first; it != m_inputSlots.begin() + last + 1; ++it) {
		delete *it;
	}
	m_inputSlots.erase(m_inputSlots.begin() + first, m_inputSlots.begin() + last + 1);
}

void Node::insertOutputSlots(int first, int last)
{
	if (first < 0 || first > outputSlotCount() || last < first)
	{
		WARN_LOG << "Invalid output slot insert: " << first << " to " << last << " (current size is " << outputSlotCount() << ")";
		return;
	}

	m_outputSlots.insert(m_outputSlots.begin() + first, last - first + 1, nullptr);
	for (auto it = m_outputSlots.begin() + first; it != m_outputSlots.begin() + last + 1; ++it) {
		OutputSlot *slot = new OutputSlot(this);
		slot->linkAdded.connect([this, slot]() { fireOutputSlotConnectEvent(slot); });
		*it = slot;
	}
	insertedOutputSlots.fire(first, last);
}

void Node::removeOutputSlots(int first, int last)
{
	if (first < 0 || last >= outputSlotCount() || last < first)
	{
		WARN_LOG << "Invalid output slot remove: " << first << " to " << last << " (size is " << outputSlotCount() << ")";
		return;
	}

	aboutToRemoveOutputSlots.fire(first, last);
	for (auto it = m_outputSlots.begin() + first; it != m_outputSlots.begin() + last + 1; ++it) {
		delete *it;
	}
	m_outputSlots.erase(m_outputSlots.begin() + first, m_outputSlots.begin() + last + 1);
}

UiElement * Node::createDelegate(UiLayout *popupLayout)
{
	return new NodeDelegate(this, popupLayout);
}

bool Node::buildRenderCommand(OutputSlot *slot, RenderCommand & cmd) const {
	for (int i = 0; i < outputSlotCount(); ++i) {
		if (slot == &outputSlot(i)) {
			return buildRenderCommand(i, cmd);
		}
	}
	return false;
}

bool Node::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	if (inputIndex >= inputSlotCount())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const Link *link = inputSlot(inputIndex).link();
	if (!link || !link->origin())
	{
		ERR_LOG << "Input " << inputIndex << " is not connected, unable to render";
		return false;
	}

	const Node *parentNode = link->origin()->parentNode();
	if (!parentNode)
	{
		ERR_LOG << "Input " << inputIndex << " is connected to an orphan slot";
		return false;
	}

	return parentNode->buildRenderCommand(link->origin(), cmd);
}

void Node::read(InputStream & stream)
{
	int n;
	Variant v;
	stream >> n;
	for (int i = 0; i < std::min(n, paramCount()); ++i)
	{
		stream >> v;
		param(i).set(v);
	}
}

void Node::write(OutputFileStream & stream) const
{
	int n = paramCount();
	stream << n;
	for (int i = 0; i < n; ++i)
	{
		stream << param(i).rawValue();
	}
}

void Node::fireInputSlotConnectEvent(InputSlot *slot) {
	// TODO: not cool to loop over all inputs, but it's ok for now
	int slotIndex;
	for (int i = 0; i < inputSlotCount(); ++i) {
		if (slot == &inputSlot(i)) {
			slotIndex = i;
		}
	}
	SlotEvent event(slotIndex);
	inputSlotConnectEvent(&event);
}

void Node::fireOutputSlotConnectEvent(OutputSlot *slot) {
	// TODO: not cool to loop over all inputs, but it's ok for now
	int slotIndex;
	for (int i = 0; i < outputSlotCount(); ++i) {
		if (slot == &outputSlot(i)) {
			slotIndex = i;
		}
	}
	SlotEvent event(slotIndex);
	outputSlotConnectEvent(&event);
}
