#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

#include <nodes/Node>

#include "GoghFlowScene.h"

using QtNodes::PortType;

GoghFlowScene::GoghFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject * parent)
	: FlowScene(registry, parent)
{
	init();
}

GoghFlowScene::GoghFlowScene(QObject * parent)
	: FlowScene(parent)
{
	init();
}

void GoghFlowScene::init() {
	m_hasBeenModified = true;

	connect(this, &FlowScene::nodeCreated, this, &GoghFlowScene::setModified);
	connect(this, &FlowScene::nodeDeleted, this, &GoghFlowScene::setModified);
	connect(this, &FlowScene::connectionCreated, this, &GoghFlowScene::setModified);
	connect(this, &FlowScene::connectionDeleted, this, &GoghFlowScene::setModified);
	connect(this, &FlowScene::nodeMoved, this, &GoghFlowScene::setModified);
	
	// TODO: Detect when DataModel's fields are modified
}

QByteArray GoghFlowScene::saveToMemory(const std::vector<Node*> & nodes) const
{
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
		return;
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

void GoghFlowScene::wasSavedAs(const QString & filename)
{
	m_filename = filename;
	m_hasBeenModified = false;
	emit fileStatusChanged(m_filename, m_hasBeenModified);
}

void GoghFlowScene::selectAll()
{
	auto allItems = items();
	auto beg = allItems.begin();
	auto end = allItems.end();
	for (auto it = beg; it != end; ++it) {
		(*it)->setSelected(true);
	}
}

void GoghFlowScene::setModified()
{
	m_hasBeenModified = true;
	emit fileStatusChanged(m_filename, m_hasBeenModified);
}
