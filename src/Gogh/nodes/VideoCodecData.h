#pragma once

#include <QString>
#include "AbstractCodecData.h"

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class VideoCodecData : public AbstractCodecData
{
public:

	VideoCodecData()
		: AbstractCodecData() {}

	VideoCodecData(const QString & name, const QStringList & options)
		: AbstractCodecData(name, options) {}

	NodeDataType type() const override
	{
		return NodeDataType {"video_codec",
		                     "Video Codec"};
	}
};
