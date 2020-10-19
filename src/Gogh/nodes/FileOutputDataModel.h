#pragma once

#include <iostream>

#include <QtCore/QObject>

#include <nodes/NodeDataModel>

#include "RenderCommand.h"

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
class VideoCodecData;
class AudioCodecData;

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
  { return true; }

  static QString
  staticName()
  { return QStringLiteral("File Output"); }

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

signals:
	/// The render command has been built and is ready for actual rendering
	void renderReady(const RenderCommand & cmd);

private slots:
	void render();

private:

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QStringLiteral("At least one stream must be provided");

  std::weak_ptr<VideoCodecData> _videoCodec;
  std::weak_ptr<AudioCodecData> _audioCodec;
  std::weak_ptr<VideoStreamData> _videoStream;
  std::weak_ptr<AudioStreamData> _audioStream;

  QWidget * _widget;
  FileInputWidget * _fileInput;
  QPushButton * _renderButton;
};
