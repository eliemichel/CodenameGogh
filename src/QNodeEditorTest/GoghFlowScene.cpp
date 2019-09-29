#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

#include <nodes/Node>

#include "GoghFlowScene.h"

using QtNodes::PortType;

GoghFlowScene::GoghFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject * parent)
	: FlowScene(registry, parent)
{}

GoghFlowScene::GoghFlowScene(QObject * parent)
	: FlowScene(parent)
{}

QByteArray GoghFlowScene::saveToMemory(const std::vector<Node*> & nodes) const {
	QJsonObject sceneJson;

	QSet<QUuid> nodeIds;

	QJsonArray nodesJsonArray;
	for (auto const & n : nodes) {
		nodesJsonArray.append(n->save());
		nodeIds.insert(n->id());
	}
	sceneJson["nodes"] = nodesJsonArray;

	QJsonArray connectionJsonArray;
	for (auto const & pair : connections())
	{
		auto const &connection = pair.second;

		auto inNodeId = connection->getNode(PortType::In)->id();
		auto outNodeId = connection->getNode(PortType::Out)->id();
		if (nodeIds.contains(inNodeId) && nodeIds.contains(outNodeId)) {
			QJsonObject connectionJson = connection->save();
			if (!connectionJson.isEmpty()) {
				connectionJsonArray.append(connectionJson);
			}
		}
	}
	sceneJson["connections"] = connectionJsonArray;

	QJsonDocument document(sceneJson);

	return document.toJson();
}

void GoghFlowScene::loadFromMemory(const QByteArray& data, bool newIds)
{
	if (!newIds) {
		FlowScene::loadFromMemory(data);
	}

	QMap<QUuid,QUuid> newIdsMap;
	QJsonObject jsonDocument = QJsonDocument::fromJson(data).object();

	QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();
	for (QJsonValueRef node : nodesJsonArray) {
		QJsonObject & nodeJson = node.toObject();
		QString oldId = nodeJson["id"].toString();
		if (!newIdsMap.contains(oldId)) {
			newIdsMap[oldId] = QUuid::createUuid();
		}
		nodeJson["id"] = newIdsMap[oldId].toString();
		node = nodeJson;
	}
	jsonDocument["nodes"] = nodesJsonArray;

	QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
	for (QJsonValueRef connection : connectionJsonArray) {
		QJsonObject & connectionJson = connection.toObject();
		QString oldId = connectionJson["in_id"].toString();
		if (newIdsMap.contains(oldId)) {
			connectionJson["in_id"] = newIdsMap[oldId].toString();
		}
		oldId = connectionJson["out_id"].toString();
		if (newIdsMap.contains(oldId)) {
			connectionJson["out_id"] = newIdsMap[oldId].toString();
		}
		connection = connectionJson;
	}
	jsonDocument["connections"] = connectionJsonArray;

	QByteArray newData = QJsonDocument(jsonDocument).toJson();
	FlowScene::loadFromMemory(newData);
}
