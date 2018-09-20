#include "Node.h"
#include "EnvModel.h"
#include "NodeGraphModel.h"
#include "DefaultNodeEditor.h"
#include "Ui/UiBase.h"
#include "Ui/NodeDelegate.h"
#include "Parameter.h"
#include "Slot.h"
#include "Logger.h"

#include <cassert>

Node::Node(QObject *parent)
	: QObject(parent)
	, m_envModel(nullptr)
	, m_graphModel(nullptr)
{
}

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

// // Legacy // //

QWidget *Node::createEditor(QWidget *parent)
{
	return new DefaultNodeEditor(this, parent);
}

bool Node::parentBuildRenderCommand(int inputIndex, RenderCommand & cmd) const
{
	if (!graphModel())
	{
		ERR_LOG << "node has no model";
	}

	if (inputIndex >= inputLinks.size())
	{
		ERR_LOG << "Input " << inputIndex << " does not exist";
		return false;
	}

	const SlotIndex & origin = inputLinks[inputIndex];
	if (!origin.isValid())
	{
		ERR_LOG << "Input " << inputIndex << " is not connected, unable to render";
		return false;
	}

	const Node *parentNode = graphModel()->node(origin.node);
	if (!parentNode)
	{
		ERR_LOG << "Input " << inputIndex << " is connected to an orphan slot";
		return false;
	}

	return parentNode->buildRenderCommand(origin.slot, cmd);
}

void Node::read(QDataStream & stream)
{
	int n;
	QVariant v;
	stream >> n;
	for (int i = 0; i < std::min(n, parmCount()); ++i)
	{
		stream >> v;
		setParm(i, v);
	}
}

void Node::write(QDataStream & stream) const
{
	int n = parmCount();
	stream << n;
	for (int i = 0; i < n; ++i)
	{
		stream << parmRawValue(i);
	}
}

void Node::fireSlotConnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotConnectEvent(&event);
}

void Node::fireSlotDisconnectEvent(int slotIndex, bool isInput)
{
	SlotEvent event(slotIndex, isInput);
	slotDisconnectEvent(&event);
}

QString Node::parmEvalAsString(int parm) const
{
	QString value;
	switch (parmType(parm))
	{
	case EnumType:
	{
		int menu = parmRawValue(parm).toInt();
		value = parmMenuValue(parm, menu).toString();
		break;
	}
	default:
		value = parmRawValue(parm).toString();
	}

	if (EnvModel *env = envModel()) {
		for (auto it = env->env().cbegin(); it != env->env().cend(); ++it)
		{
			value = value.replace(QString::fromStdString("$" + it->first), QString::fromStdString(it->second));
		}
	}
	return value;
}

int Node::parmEvalAsInt(int parm) const
{
	QString value;
	switch (parmType(parm))
	{
	case EnumType:
	{
		int menu = parmRawValue(parm).toInt();
		return parmMenuValue(parm, menu).toInt();
		break;
	}
	default:
		return parmRawValue(parm).toInt();
	}
}

bool Node::parmEvalAsBool(int parm) const
{
	QString value;
	switch (parmType(parm))
	{
	case EnumType:
	{
		int menu = parmRawValue(parm).toBool();
		return parmMenuValue(parm, menu).toBool();
		break;
	}
	default:
		return parmRawValue(parm).toBool();
	}
}

void Node::newInputSlot()
{
	inputLinks.resize(inputLinks.size() + 1);
	if (graphModel())
	{
		graphModel()->broadcastNodeChange(nodeIndex());
	}
}

void Node::newOutputSlot()
{
	outputLinks.resize(outputLinks.size() + 1);
	if (graphModel())
	{
		graphModel()->broadcastNodeChange(nodeIndex());
	}
}

void Node::removeOutputSlots()
{
	if (!graphModel())
	{
		return;
	}

	// Remove links
	for (std::set<SlotIndex> & destinationSet : outputLinks)
	{
		while (!destinationSet.empty())
		{
			graphModel()->removeLink(*destinationSet.begin());
		}
	}

	// Remove slots
	// TODO: remove graphicsslots
	outputLinks.clear();

	graphModel()->broadcastNodeChange(nodeIndex());
}

Connection Node::inputConnection(int inputSlotIndex)
{
	if (inputSlotIndex < 0 || inputSlotIndex > inputSlotCount_legacy())
	{
		ERR_LOG << "Invalid input slot index: #" << inputSlotIndex;
		return Connection();
	}
	const SlotIndex & sid = inputLinks[inputSlotIndex];
	if (graphModel() && sid.isValid())
	{
		return Connection(graphModel()->node(sid.node), sid.slot);
	}
	else
	{
		return Connection();
	}
}

std::set<Connection> Node::outputConnection(int outputSlotIndex)
{
	if (outputSlotIndex < 0 || outputSlotIndex > outputSlotCount_legacy())
	{
		ERR_LOG << "Invalid output slot index: #" << outputSlotIndex;
		return std::set<Connection>();
	}
	const std::set<SlotIndex> & slotSet = outputLinks[outputSlotIndex];
	if (graphModel())
	{
		std::set<Connection> connectionSet;
		for (auto sid : slotSet)
		{
			connectionSet.insert(sid.isValid() ? Connection(graphModel()->node(sid.node), sid.slot) : Connection());
		}
		return connectionSet;
	}
	else
	{
		return std::set<Connection>();
	}
}

std::string Node::streamTypeAsString(StreamType stream) const
{
	switch (stream)
	{
	case VideoStream:
		return "VideoStream";
	case AudioStream:
		return "AudioStream";
	case SubtitleStream:
		return "SubtitleStream";
	case DataStream:
		return "DataStream";
	default:
	return "No Stream";
	}
}

char Node::streamTypeAsChar(StreamType stream) const
{
	switch (stream)
	{
	case VideoStream:
		return 'v';
	case AudioStream:
		return 'a';
	case SubtitleStream:
		return 's';
	case DataStream:
		return 'd';
	default:
		return '-';
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
	SlotEvent event(slotIndex, true /* isInput */);
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
	SlotEvent event(slotIndex, false /* isInput */);
	outputSlotConnectEvent(&event);
}
