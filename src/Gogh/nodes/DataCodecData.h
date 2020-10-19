#pragma once

#include <QString>
#include "AbstractCodecData.h"

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class DataCodecData : public AbstractCodecData
{
public:

	DataCodecData()
		: AbstractCodecData() {}

	DataCodecData(const QString & name, const QStringList & options)
		: AbstractCodecData(name, options) {}

	NodeDataType type() const override
	{
		return NodeDataType {"data_codec",
		                     "Data Codec"};
	}
};
