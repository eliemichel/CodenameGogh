#pragma once

#include "AbstractStreamData.h"

using QtNodes::NodeDataType;

class AudioStreamData : public AbstractStreamData
{
public:
  AudioStreamData()
    : AbstractStreamData()
  {}

  AudioStreamData(const QString & filename, int streamId)
    : AbstractStreamData(filename, streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"audio",
                         "Audio"};
  }

  QString ffmpegShortName() const override
  { return "a"; }
};
