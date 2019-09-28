#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class AudioStreamData : public NodeData
{
public:

  AudioStreamData()
    : _fileId(-1)
    , _streamId(-1)
  {}

  AudioStreamData(int fileId, int streamId)
    : _fileId(fileId)
    , _streamId(streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"audio",
                         "Audio"};
  }

  int fileId() const
  { return _fileId; }

  int streamId() const
  { return _streamId; }

private:
  int _fileId;
  int _streamId;
};
