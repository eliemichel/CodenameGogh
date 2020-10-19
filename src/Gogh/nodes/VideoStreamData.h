#pragma once

#include "AbstractStreamData.h"

using QtNodes::NodeDataType;

class VideoStreamData : public AbstractStreamData
{
public:
  VideoStreamData()
    : AbstractStreamData()
  {}

  VideoStreamData(const QString & filename, int streamId)
    : AbstractStreamData(filename, streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"video",
                         "Video"};
  }

  QString ffmpegShortName() const override
  { return "v"; }
};
