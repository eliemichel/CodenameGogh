#include "RenderProcess.h"
#include "Logger.h"
#include "MainEventLoop.h"

#include <iostream>
#include <thread>
#include <future>

std::string RenderProcess::locateFfmpeg()
{
	return "ffmpeg";
}

RenderProcess::RenderProcess(const std::vector<std::string> & args)
	: m_args(args)
{
	processFinished.connect(this, &RenderProcess::onProcessFinished);
}

void RenderProcess::start()
{
	std::string cmd = locateFfmpeg();
	for (const std::string & arg : m_args) {
		cmd += " " + arg;
	}

	DEBUG_LOG << cmd;
	
	m_process = std::make_shared<TinyProcessLib::Process>(cmd, "", [this](const char *bytes, size_t n) {
		std::cout << std::string(bytes, n);
	}, [](const char *bytes, size_t n) {
		WARN_LOG << std::string(bytes, n);
	});

	std::packaged_task<int()> task([this]() {
		return m_process->get_exit_status();
	});
	std::future<int> future = task.get_future();
	std::thread(std::move(task)).detach();
	MainEventLoop::GetInstance().Add(std::move(future), &processFinished);
}

bool RenderProcess::waitForFinished()
{
	return m_process->get_exit_status() == 0;
}

void RenderProcess::onProcessFinished(int exit_status)
{
	LOG << "render finished with code " << exit_status;
	rendered.fire();
}
