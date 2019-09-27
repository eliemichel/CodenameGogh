#include <QLineEdit>

#include "FileOutputDataModel.h"

#include "VideoStreamData.h"
#include "AudioStreamData.h"

FileOutputDataModel::
FileOutputDataModel()
  : _lineEdit(new QLineEdit())
{}


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
    case 1:
    default:
      _audioStream = std::dynamic_pointer_cast<AudioStreamData>(data);
  }

  auto videoStreamData = _videoStream.lock();
  auto audioStreamData = _audioStream.lock();
  if (videoStreamData && audioStreamData)
  {
    modelValidationState = NodeValidationState::Valid;
    modelValidationError = QString();
    std::cout
      << "Exporting video stream #" << videoStreamData->streamId() << " from input file #" << videoStreamData->fileId()
      << " and audio stream #" << audioStreamData->streamId() << " from input file #" << audioStreamData->fileId() << std::endl;
  }
  else
  {
    modelValidationState = NodeValidationState::Warning;
    modelValidationError = QStringLiteral("Missing or incorrect inputs");
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
