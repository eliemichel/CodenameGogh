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
