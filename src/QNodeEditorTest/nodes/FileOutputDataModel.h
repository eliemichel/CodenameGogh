#pragma once

#include <QtCore/QObject>

#include <nodes/NodeDataModel>

#include <iostream>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class FileInputWidget;
class QPushButton;
class VideoStreamData;
class AudioStreamData;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class FileOutputDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  FileOutputDataModel();

  virtual
  ~FileOutputDataModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("File Output"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("File Output"); }

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType,
           PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data, int portIndex) override;

  QWidget *
  embeddedWidget() override { return _widget; }

  NodeValidationState
  validationState() const override;

  QString
  validationMessage() const override;

private slots:
	void render() const;

private:

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QStringLiteral("At least one stream must be provided");

  std::weak_ptr<VideoStreamData> _videoStream;
  std::weak_ptr<AudioStreamData> _audioStream;

  QWidget * _widget;
  FileInputWidget * _fileInput;
  QPushButton * _renderButton;
};
