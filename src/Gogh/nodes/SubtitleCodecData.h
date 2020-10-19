#pragma once

#include <QString>
#include "AbstractCodecData.h"

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class SubtitleCodecData : public AbstractCodecData
{
public:

	SubtitleCodecData()
		: AbstractCodecData() {}

	SubtitleCodecData(const QString & name, const QStringList & options)
		: AbstractCodecData(name, options) {}

	NodeDataType type() const override
	{
		return NodeDataType {"subtitle_codec",
		                     "Subtitle Codec"};
	}
};
