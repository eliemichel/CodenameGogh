#pragma once

#include <QString>
#include "AbstractCodecData.h"

using QtNodes::NodeDataType;
using QtNodes::NodeData;

class AudioCodecData : public AbstractCodecData
{
public:

	AudioCodecData()
		: AbstractCodecData() {}

	AudioCodecData(const QString & name, const QStringList & options)
		: AbstractCodecData(name, options) {}

	NodeDataType type() const override
	{
		return NodeDataType {"audio_codec",
		                     "Audio Codec"};
	}
};
