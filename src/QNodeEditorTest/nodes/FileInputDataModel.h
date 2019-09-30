#pragma once

#include <QtCore/QObject>

#include <nodes/NodeDataModel>

#include "FileProbeProcess.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class VideoStreamData;
class FileInputWidget;

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
  { return true; }

  static QString
  staticName()
  { return QStringLiteral("File Input"); }

  QString
  name() const override
  { return staticName(); }

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
  embeddedWidget() override { return _widget; }

private Q_SLOTS:

  void onFileChanged(QString const &filename);
  void onFileProbed();

private:
  std::vector<std::shared_ptr<NodeData>> _streams;

  QWidget * _widget;
  FileInputWidget * _fileInput;
  FileProbeProcess _probeProcess;
};
