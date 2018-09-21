#ifndef H_FILESTREAM
#define H_FILESTREAM

#include <fstream>

class InputStream : public std::ifstream {
public:
	InputStream(std::string filename)
		: m_stream(filename, std::ios::in | std::ios::binary)
	{}

	~InputStream() {
		m_stream.close();
	}

	template <typename T>
	InputStream & operator>>(T& value) {
		m_stream.read((char*)&value, sizeof(T));
		return *this;
	}

private:
	std::ifstream m_stream;
};

class OutputFileStream {
public:
	OutputFileStream(std::string filename)
		: m_stream(filename, std::ios::out | std::ios::binary)
	{}

	~OutputFileStream() {
		m_stream.close();
	}

	template <typename T>
	OutputFileStream & operator<<(const T& value) {
		m_stream.write((char*)&value, sizeof(T));
		return *this;
	}

private:
	std::ofstream m_stream;
};

#endif // H_FILESTREAM

