#pragma once

#include <QString>
#include <nodes/NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class AbstractCodecData : public NodeData
{
public:

  AbstractCodecData()
    : _name("copy")
  {}

  AbstractCodecData(const QString & name, const QStringList & options)
    : _name(name)
    , _options(options)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"codec",
                         "Codec"};
  }

  const QString & name() const
  { return _name; }

  const QStringList & options() const
  { return _options; }

  QStringList & options()
  { return _options; }

private:
  QString _name;
  QStringList _options;
};
