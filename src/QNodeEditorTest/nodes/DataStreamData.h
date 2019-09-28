#pragma once

#include "AbstractStreamData.h"

using QtNodes::NodeDataType;

class DataStreamData : public AbstractStreamData
{
public:
  DataStreamData()
    : AbstractStreamData()
  {}

  DataStreamData(const QString & filename, int streamId)
    : AbstractStreamData(filename, streamId)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"data",
                         "Data"};
  }

  QString ffmpegShortName() const override
  { return "d"; }
};
