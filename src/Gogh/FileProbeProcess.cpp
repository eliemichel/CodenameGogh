#include "FileProbeProcess.h"
#include "Logger.h"
#include "MainEventLoop.h"

#include "utils/strutils.h"

#include <iostream>
#include <string>
#include <thread>
#include <future>

std::string FileProbeProcess::locateFfprobe()
{
	return "ffprobe";
}

FileProbeProcess::FileProbeProcess()
	: m_wasCanceled(true)
	, m_isRunning(false)
	, m_mustRetry(false)
{
	processFinished.connect(this, &FileProbeProcess::onProcessFinished);
}

void FileProbeProcess::probe(std::string filename)
{
	m_filename = filename;
	if (m_isRunning)
	{
		m_mustRetry = true;
		cancel();
		return;
	}

	m_wasCanceled = false;
	m_isRunning = true;
	m_mustRetry = false;

	std::string cmd =
		locateFfprobe()
		+ " -v error"
		+ " -show_entries stream=codec_type"
		+ " -print_format"
		+ " default=noprint_wrappers=1:nokey=1"
		+ " " + m_filename;

	DEBUG_LOG << cmd;

	m_process = std::make_shared<TinyProcessLib::Process>(cmd, "", [this](const char *bytes, size_t n) {
		m_stdin << std::string(bytes, n);
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

void FileProbeProcess::cancel()
{
	m_wasCanceled = false;
	if (m_process) {
		m_process->kill();
	}
}

void FileProbeProcess::onProcessFinished(int exit_status)
{
	m_exit_status = exit_status;
	m_isRunning = false;
	LOG << "probe finished with code " << m_exit_status << " (" << (m_exit_status == 0 ? "success" : "failure") << ")";
	if (m_wasCanceled)
	{
		if (m_mustRetry)
		{
			probe(m_filename);
		}
	}

	// Parse output
	m_streams.clear();
	std::string line;
	do
	{
		std::getline(m_stdin, line);
		trim(line);
		if (line.empty())
		{
			continue;
		}
		if (line == "video")
		{
			m_streams.push_back(VideoStream);
		}
		else if (line == "audio")
		{
			m_streams.push_back(AudioStream);
		}
		else if (line == "subtitle")
		{
			m_streams.push_back(SubtitleStream);
		}
		else if (line == "data") // TODO: confirm that it is "data"
		{
			m_streams.push_back(DataStream);
		}
	} while (!m_stdin.eof());

	probed.fire();
}
