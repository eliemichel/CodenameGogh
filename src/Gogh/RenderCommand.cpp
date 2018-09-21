#include "RenderCommand.h"

// Overloaders
std::ostream& operator<<(std::ostream& stream, const filestream& fs)
{
	stream << fs.first << " -> " << fs.second;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const OutputStream& os)
{
	stream << "Input filestream: " << os.input.first << " -> " << os.input.second << "\n"
		<< "Stream Type: " << os.stream << "\n"
		<< "Name: " << os.name << "\n"
		<< "Settings: " << os.settings;
	return stream;
}

std::string streamTypeAsString(StreamType stream)
{
	switch (stream)
	{
	case VideoStream:
		return "VideoStream";
	case AudioStream:
		return "AudioStream";
	case SubtitleStream:
		return "SubtitleStream";
	case DataStream:
		return "DataStream";
	default:
		return "No Stream";
	}
}

char streamTypeAsChar(StreamType stream)
{
	switch (stream)
	{
	case VideoStream:
		return 'v';
	case AudioStream:
		return 'a';
	case SubtitleStream:
		return 's';
	case DataStream:
		return 'd';
	default:
		return '-';
	}
}
