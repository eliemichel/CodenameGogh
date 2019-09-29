#include <iostream>

#include <QJsonValue>
#include <QJsonArray>
#include <QIntValidator>
#include <QVBoxLayout>

#include "FileInputDataModel.h"
#include "VideoStreamData.h"
#include "AudioStreamData.h"
#include "SubtitleStreamData.h"
#include "DataStreamData.h"
#include "widgets/FileInputWidget.h"

FileInputDataModel::
FileInputDataModel()
  : _widget(new QWidget())
  , _fileInput(new FileInputWidget(FileInputWidget::ReadFile))
{
  connect(_fileInput, &FileInputWidget::fileChanged, this, &FileInputDataModel::onFileChanged);
  connect(&_probeProcess, &FileProbeProcess::probed, this, &FileInputDataModel::onFileProbed);

  auto layout = new QVBoxLayout(_widget);
  layout->setMargin(0);
  layout->addWidget(_fileInput);
  _widget->setStyleSheet("QWidget{background-color: rgba(0,0,0,0);color: white} QAbstractButton{background-color: rgba(96,96,96,204)}");
}

QJsonObject
FileInputDataModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  modelJson["file"] = _fileInput->filename();
  QJsonArray jsonStreams;
  for (auto s : _streams) {
	  jsonStreams << s->type().id;
  }
  modelJson["streams"] = jsonStreams;

  return modelJson;
}


void
FileInputDataModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["file"];

  if (!v.isUndefined())
  {
	_fileInput->setFilename(v.toString());
  }

  QJsonArray jsonStreams;
  for (auto s : _streams) {
	  jsonStreams << s->type().id;
  }
  QJsonValue s = p["streams"];
  if (s.isArray()) {
	  _streams.clear();
	  int i = 0;
	  for (auto value : s.toArray()) {
		QString stream = value.toString();
		if (stream == VideoStreamData().type().id) {
			_streams.push_back(std::make_shared<VideoStreamData>(_fileInput->filename(), i));
		} else if (stream == AudioStreamData().type().id) {
			_streams.push_back(std::make_shared<AudioStreamData>(_fileInput->filename(), i));
		} else if (stream == SubtitleStreamData().type().id) {
			_streams.push_back(std::make_shared<SubtitleStreamData>(_fileInput->filename(), i));
		} else if (stream == DataStreamData().type().id) {
			_streams.push_back(std::make_shared<DataStreamData>(_fileInput->filename(), i));
		} else {
			std::cerr << "Invalid stream type: " << stream.toStdString() << std::endl;
		}
		++i;
	  }

	  Q_EMIT portCountChanged();
  }
}


unsigned int
FileInputDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = static_cast<unsigned int>(_streams.size());

    default:
      break;
  }

  return result;
}

void
FileInputDataModel::
onFileChanged(QString const &filename)
{
	_probeProcess.probe(filename);
}

void
FileInputDataModel::
onFileProbed()
{
	_streams.clear();
	int i = 0;
	for (auto stream : _probeProcess.streams()) {
		switch (stream)
		{
		case FileProbeProcess::VideoStream:
			_streams.push_back(std::make_shared<VideoStreamData>(_fileInput->filename(), i));
			break;
		case FileProbeProcess::AudioStream:
			_streams.push_back(std::make_shared<AudioStreamData>(_fileInput->filename(), i));
			break;
		case FileProbeProcess::SubtitleStream:
			_streams.push_back(std::make_shared<SubtitleStreamData>(_fileInput->filename(), i));
			break;
		case FileProbeProcess::DataStream:
			_streams.push_back(std::make_shared<DataStreamData>(_fileInput->filename(), i));
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
		++i;
	}
	Q_EMIT portCountChanged();
	for (int i = 0 ; i < static_cast<int>(_streams.size()) ; ++i) {
		Q_EMIT dataUpdated(i);
	}
}

NodeDataType
FileInputDataModel::
dataType(PortType, PortIndex portIndex) const
{
	if (portIndex >= 0 && portIndex < static_cast<int>(_streams.size())) {
		return _streams[portIndex]->type();
	} else {
		return VideoStreamData().type();
	}
}

std::shared_ptr<NodeData>
FileInputDataModel::
outData(PortIndex port)
{
	return _streams[port];
}