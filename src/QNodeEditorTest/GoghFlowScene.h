#pragma once

#include <nodes/FlowScene>

using QtNodes::FlowScene;
using QtNodes::Node;
using QtNodes::DataModelRegistry;

class GoghFlowScene : public FlowScene {
	Q_OBJECT
public:
	GoghFlowScene(std::shared_ptr<DataModelRegistry> registry,
                  QObject * parent = nullptr);

	GoghFlowScene(QObject * parent = nullptr);

public:
	/// Save only some of the nodes to memory
	QByteArray saveToMemory(const std::vector<Node*> & nodes) const;

	/// if newIds is true, replace uuids in data by new ones to avoid conflit
	/// with existing nodes.
	void loadFromMemory(const QByteArray& data, bool newIds);
};
