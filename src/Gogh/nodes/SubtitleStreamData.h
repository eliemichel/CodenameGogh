#pragma once

#include "AbstractStreamData.h"

using QtNodes::NodeDataType;

class SubtitleStreamData : public AbstractStreamData
{
public:
  SubtitleStreamData()
    : AbstractStreamData()
  {}

  SubtitleStreamData(const QString & filename, int streamId)
    : AbstractStreamData(filename, streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"subtitle",
                         "Subtitle"};
  }

  QString ffmpegShortName() const override
  { return "s"; }
};
