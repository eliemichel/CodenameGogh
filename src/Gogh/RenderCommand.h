#ifndef H_RENDERCOMMAND
#define H_RENDERCOMMAND

#include "utils/stringlist.h"
#include "utils/strutils.h"
#include <vector>
#include <string>
#include <map>

/** Type defined to transmit a pair (input_file, stream_number)
which characterizes a stream.
*/
typedef std::pair<std::string, int> filestream;

std::ostream& operator<<(std::ostream& stream, const filestream& fs);

enum StreamType
{
	VideoStream,
	AudioStream,
	SubtitleStream,
	DataStream,
};

std::string streamTypeAsString(StreamType stream);

char streamTypeAsChar(StreamType stream);

/**
* This structure constains everything that defines an output stream
* in the rendered file.
*/
struct OutputStream {
	// Input filestream
	filestream input;

	// StreamType
	StreamType stream;

	// Output stream name
	std::string name;

	// Settings
	stringlist settings;
};

std::ostream& operator<<(std::ostream& stream, const OutputStream& fs);

/**
* This structure is transmitted among the graph nodes while building the
* render command. Feel free to add any field required to properly build
* the command.
*/
struct RenderCommand {
	// == Input ==
	// Variables used for input treatment

	// OutputNode's smart renaming : keys (like : "codec", "scale") associated to current nodes values (like : "h264", "1920x1080")
	std::map<std::string, std::string> env;

	// Current output stream, every node action must be written in it
	OutputStream os;

	//Input sources
	stringlist sources;

	// ------------
	// == Output ==
	// Variables used for output command building

	// Filestream by output streams
	std::map<int, OutputStream> outputs;

	// Final command to send to ffmpeg, must be filled only int the OuputNode
	stringlist cmd;

	// ------------
	// == Functions ==
	// Return the ID of a source regarding an input filestream
	int source_id(filestream filestream) {
		for (int i = 0; i < sources.size(); i++)
		{
			if (filestream.first == sources[i])
			{
				return i;
			}
		}
		return 0;
	}

	// Sort outputs in the pattern : v - a - s - d
	void sort_outputs()
	{
		std::vector<OutputStream> sort;
		// Order streams
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < outputs.size(); j++)
			{
				if (int(outputs[j].stream) == i)
				{
					sort.push_back(outputs[j]);
				}
			}
		}
		// Set ordered streams in outputs
		for (int i = 0; i < outputs.size(); i++)
		{
			outputs[i] = sort[i];
		}
	}

	// ------------
	// == Other ==
	// error message that may be filled when returning false in buildRenderCommand
	std::string err;
};


#endif // H_RENDERCOMMAND
