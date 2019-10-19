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
	/**
	 * Path of this scene file. It is empty if and only if the scene has not
	 * been saved yet.
	 */
	QString filename() const { return m_filename; }

	/**
	 * Tells whether the scene been modified since it has last been saved or
	 * loaded.
	 */
	bool hasBeenModified() const { return m_hasBeenModified; }

	/**
	 * Save only some of the nodes to memory
	 */
	QByteArray saveToMemory(const std::vector<Node*> & nodes) const;

	/**
	 * if newIds is true, replace uuids in data by new ones to avoid conflit
	 * with existing nodes.
	 */
	void loadFromMemory(const QByteArray& data, bool newIds = false);

signals:
	/**
	 * Emitted whenever filename() or hasBeenModified() changes
	 */
	void fileStatusChanged(const QString & filename, bool hasBeenModified);

public slots:
	/**
	 * Call this to signal the scene that it has been successfully saved. This
	 * updates filename() and hasBeenModified()
	 */
	void wasSavedAs(const QString & filename);

	void selectAll();

private:
	/**
	 * Common elements of constructors
	 */
	void init();

private slots:
	/**
	 * Flag the scene as beeing modified since last save/load, and emit the
	 * fileStatusChanged signal.
	 */
	void setModified();

private:
	QString m_filename;
	bool m_hasBeenModified;
};
