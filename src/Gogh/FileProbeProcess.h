#ifndef H_FILEPROBEPROCESS
#define H_FILEPROBEPROCESS

#include "Node.h"
#include "Signal.h"
#include "process.hpp"

#include <thread>
#include <vector>
#include <string>

class FileProbeProcess
{
public:
	static std::string locateFfprobe();

public:
	FileProbeProcess();

	// If a job is running, this will interupt it
	void probe(std::string filename);
	void cancel();

	const std::vector<StreamType> & streams() const { return m_streams; }

public: // signals
	Signal<> probed;

private: // signals
	Signal<> processFinished;

private: // slots
	void onProcessFinished();

private:
	std::shared_ptr<TinyProcessLib::Process> m_process;

	std::vector<StreamType> m_streams;
	bool m_wasCanceled;
	bool m_isRunning;
	bool m_mustRetry;
	std::string m_filename;
};

#endif // H_FILEPROBEPROCESS
