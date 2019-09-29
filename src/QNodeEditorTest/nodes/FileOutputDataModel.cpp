#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>

#include "FileOutputDataModel.h"
#include "widgets/FileInputWidget.h"
#include "RenderCommand.h"

#include "VideoStreamData.h"
#include "AudioStreamData.h"

FileOutputDataModel::
FileOutputDataModel()
	: _widget(new QWidget())
	, _fileInput(new FileInputWidget(FileInputWidget::WriteFile))
	, _renderButton(new QPushButton("Render"))
{
	connect(_renderButton, &QPushButton::clicked, this, &FileOutputDataModel::render);
	auto layout = new QHBoxLayout(_widget);
	layout->addWidget(_fileInput);
	layout->addWidget(_renderButton);
	_widget->setStyleSheet("QWidget{background-color: rgba(0,0,0,0);color: white} QAbstractButton{background-color: rgba(96,96,96,204)}");
}

QJsonObject
FileOutputDataModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  modelJson["file"] = _fileInput->filename();

  return modelJson;
}


void
FileOutputDataModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["file"];

  if (!v.isUndefined())
  {
	_fileInput->setFilename(v.toString());
  }
}

unsigned int
FileOutputDataModel::
nPorts(PortType portType) const
{
  switch (portType)
  {
    case PortType::In:
      return 2;

    case PortType::Out:
      return 0;

    default:
      return 0;
  }
}


NodeDataType
FileOutputDataModel::
dataType(PortType, PortIndex portIndex) const
{
  switch (portIndex) {
    case 0:
      return VideoStreamData().type();
    case 1:
    default:
      return AudioStreamData().type();
  }
}


std::shared_ptr<NodeData>
FileOutputDataModel::
outData(PortIndex)
{
  std::shared_ptr<NodeData> ptr;
  return ptr;
}


void
FileOutputDataModel::
setInData(std::shared_ptr<NodeData> data, int portIndex)
{
  switch (portIndex) {
    case 0:
      _videoStream = std::dynamic_pointer_cast<VideoStreamData>(data);
	  break;
    case 1:
    default:
      _audioStream = std::dynamic_pointer_cast<AudioStreamData>(data);
  }

  auto videoStreamData = _videoStream.lock();
  auto audioStreamData = _audioStream.lock();
  if (videoStreamData || audioStreamData)
  {
    modelValidationState = NodeValidationState::Valid;
    modelValidationError = QString();
  }
  else
  {
    modelValidationState = NodeValidationState::Warning;
    modelValidationError = QStringLiteral("At least one stream must be provided");
  }

  // Update default directory
  QString dir = QDir::currentPath();
  QString inputFile;
  if (videoStreamData) inputFile = videoStreamData->filename();
  if (audioStreamData) inputFile = audioStreamData->filename();
  if (!inputFile.isEmpty()) {
    QFileInfo info(inputFile);
    if (info.isDir()) dir = info.path();
    if (info.isFile()) dir = info.dir().path();
    _fileInput->setDefaultDir(dir);
  }
}


NodeValidationState
FileOutputDataModel::
validationState() const
{
  return modelValidationState;
}


QString
FileOutputDataModel::
validationMessage() const
{
  return modelValidationError;
}

void
FileOutputDataModel::
render()
{
  auto videoStreamData = _videoStream.lock();
  auto audioStreamData = _audioStream.lock();

  // streams to export in the file
  QList<std::shared_ptr<AbstractStreamData>> outputStreams;

  if (videoStreamData && audioStreamData)
  {
    std::cout
      << "Exporting video stream #" << videoStreamData->streamId() << " from input file " << videoStreamData->filename().toStdString()
      << " and audio stream #" << audioStreamData->streamId() << " from input file #" << audioStreamData->filename().toStdString() << std::endl;

	outputStreams << videoStreamData;
	outputStreams << audioStreamData;
  }
  else if (videoStreamData)
  {
    std::cout
      << "Exporting only video stream #" << videoStreamData->streamId() << " from input file " << videoStreamData->filename().toStdString() << std::endl;
	outputStreams << videoStreamData;
  }
  else if (audioStreamData)
  {
    std::cout
      << "Exporting only audio stream #" << audioStreamData->streamId() << " from input file " << audioStreamData->filename().toStdString() << std::endl;
	outputStreams << audioStreamData;
  }
  else
  {
    std::cout << "Nothing to render" << std::endl;
  }

  if (!outputStreams.isEmpty()) {
	  RenderCommand cmd(_fileInput->filename(), outputStreams);
	  emit renderReady(cmd);
  }
}
