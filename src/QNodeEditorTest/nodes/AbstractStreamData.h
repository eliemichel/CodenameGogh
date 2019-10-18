#pragma once

#include <QString>
#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class AbstractStreamData : public NodeData
{
public:

  AbstractStreamData()
    : _filename("")
    , _streamId(-1)
  {}

  AbstractStreamData(const QString & filename, int streamId)
    : _filename(filename)
    , _streamId(streamId)
  {}

  virtual QString ffmpegShortName() const = 0; // v, a, s or d

  QString filename() const
  { return _filename; }

  int streamId() const
  { return _streamId; }

  QStringList & options()
  { return _options; }

private:
	/// Filename from which this stream is loaded
	QString _filename;

	/// Input options for reading the file, e.g. image sequence -start_number
	QStringList _options;

	/// Id of the stream within the file.
	int _streamId;
};
