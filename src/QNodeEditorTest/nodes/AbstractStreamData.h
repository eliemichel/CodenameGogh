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

private:
  QString _filename;
  int _streamId;
};
