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
      << "Exporting video stream #" << videoStreamData->streamId() << " from input file " << videoStreamData->filename().toStdString()
      << " and audio stream #" << audioStreamData->streamId() << " from input file #" << audioStreamData->filename().toStdString() << std::endl;

	// TODO: move this into a dedicated location
	QVector<std::shared_ptr<AbstractStreamData>> outputStreams{
		videoStreamData,
		audioStreamData
	};

	QVector<int> streams(outputStreams.size());
	QVector<int> streamsIndexAmongType(outputStreams.size()); // stream 'i' is the streamsIndexAmongType[i]th stream of its type (video, audio, etc.)
	QMap<QString, int> streamCountPerType;

	QList<QString> inputFilenames;
	QMap<QString, int> inputFilenamesMap;

	for (int i = 0 ; i < outputStreams.size() ; ++i) {
		const QString & filename = outputStreams[i]->filename();
		if (!inputFilenamesMap.contains(filename)) {
			inputFilenamesMap[filename] = inputFilenamesMap.count();
			inputFilenames << filename;
		}
		streams[i] = inputFilenamesMap[filename];

		const QString & sname = outputStreams[i]->ffmpegShortName();
		if (!streamCountPerType.contains(sname)) streamCountPerType[sname] = 0;
		streamsIndexAmongType[i] = streamCountPerType[sname];
		streamCountPerType[sname]++;
	}

	QString cmd = "ffmpeg";
	for (const auto & f : inputFilenames) {
		cmd += " -i " + f; // TODO: escape string
	}
	for (int i = 0 ; i < outputStreams.size() ; ++i) {
		cmd += " -map " + QString::number(streams[i]) + ":" + QString::number(outputStreams[i]->streamId());
	}
	for (int i = 0 ; i < outputStreams.size() ; ++i) {
		const QString & sname = outputStreams[i]->ffmpegShortName();
		cmd += " -c:" + sname + ":" + QString::number(streamsIndexAmongType[i]) + " copy";
	}
	cmd += " " + _fileInput->filename(); // TODO: escape string
	std::cout << "CMD: " << cmd.toStdString() << std::endl;
  }
  else if (videoStreamData)
  {
    std::cout
      << "Exporting only video stream #" << videoStreamData->streamId() << " from input file " << videoStreamData->filename().toStdString() << std::endl;
  }
  else if (audioStreamData)
  {
    std::cout
      << "Exporting only audio stream #" << audioStreamData->streamId() << " from input file " << audioStreamData->filename().toStdString() << std::endl;
  }
  else
  {
    std::cout << "Nothing to render" << std::endl;
  }
}
