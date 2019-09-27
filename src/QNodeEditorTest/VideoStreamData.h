#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class VideoStreamData : public NodeData
{
public:

  VideoStreamData()
    : _fileId(-1)
    , _streamId(-1)
  {}

  VideoStreamData(int fileId, int streamId)
    : _fileId(fileId)
    , _streamId(streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"video",
                         "Video"};
  }

  int fileId() const
  { return _fileId; }

  int streamId() const
  { return _streamId; }

private:
  int _fileId;
  int _streamId;
};
