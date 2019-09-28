#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

#include "FileOutputDataModel.h"
#include "widgets/FileInputWidget.h"

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
render() const
{
  auto videoStreamData = _videoStream.lock();
  auto audioStreamData = _audioStream.lock();
  if (videoStreamData && audioStreamData)
  {
    std::cout
      << "Exporting video stream #" << videoStreamData->streamId() << " from input file #" << videoStreamData->fileId()
      << " and audio stream #" << audioStreamData->streamId() << " from input file #" << audioStreamData->fileId() << std::endl;
  }
  else if (videoStreamData)
  {
    std::cout
      << "Exporting only video stream #" << videoStreamData->streamId() << " from input file #" << videoStreamData->fileId() << std::endl;
  }
  else if (audioStreamData)
  {
    std::cout
      << "Exporting only audio stream #" << audioStreamData->streamId() << " from input file #" << audioStreamData->fileId() << std::endl;
  }
  else
  {
    std::cout << "Nothing to render" << std::endl;
  }
}
