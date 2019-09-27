#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include <nodes/NodeDataModel>

#include <iostream>

class VideoStreamData;

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class FileInputDataModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  FileInputDataModel();

  virtual
  ~FileInputDataModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("File Input"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("FileInput"); }

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData>, int) override
  { }

  QWidget *
  embeddedWidget() override { return _lineEdit; }

private:
	void setStreamCount(int number);

private Q_SLOTS:

  void
  onTextEdited(QString const &string);

private:

  std::vector<std::shared_ptr<VideoStreamData>> _streams;

  QLineEdit * _lineEdit;
};
