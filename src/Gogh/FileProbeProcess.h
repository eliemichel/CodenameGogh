#ifndef H_FILEPROBEPROCESS
#define H_FILEPROBEPROCESS

#include "Node.h"
#include "Signal.h"
#include "process.hpp"

#include <thread>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

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
	Signal<int> processFinished; // (int exit_status)

private: // slots
	void onProcessFinished(int exit_status);

private:
	std::shared_ptr<TinyProcessLib::Process> m_process;
	int m_exit_status; // written in process only
	std::stringstream m_stdin;

	std::vector<StreamType> m_streams;
	bool m_wasCanceled;
	bool m_isRunning;
	bool m_mustRetry;
	std::string m_filename;
};

#endif // H_FILEPROBEPROCESS
