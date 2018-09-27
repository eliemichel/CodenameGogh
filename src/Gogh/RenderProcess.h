#ifndef H_RENDERPROCESS
#define H_RENDERPROCESS

#include "Object.h"
#include "Signal.h"
#include "process.hpp"

#include <memory>
#include <sstream>

class RenderProcess : public Object
{
public:
	static std::string locateFfmpeg();

public:
	RenderProcess(const std::vector<std::string> & args);

	void start();
	bool waitForFinished();

public: // signals
	Signal<> rendered;

private: // signals
	Signal<int> processFinished; // (int exit_status)

private: // slots
	void onProcessFinished(int exit_status);

private:
	std::vector<std::string> m_args;
	std::shared_ptr<TinyProcessLib::Process> m_process;
	int m_exit_status; // written in process only
	std::stringstream m_stdin;
};

#endif // H_RENDERPROCESS
