#include "FileProbeProcess.h"
#include "Logger.h"

#include <iostream>
#include <string>

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
		+ "-v error"
		+ "-show_entries stream=codec_type"
		+ "-print_format"
		+ "default=noprint_wrappers=1:nokey=1"
		+ m_filename;

	DEBUG_LOG << cmd;

	delete m_process;

	m_process = make_shared<TinyProcessLib::Process>("ffmpeg -version", "", [](const char *bytes, size_t n) {
		std::cout << "Output from stdout: " << std::string(bytes, n);
	}, [](const char *bytes, size_t n) {
		WARN_LOG << std::string(bytes, n);
	});

	std::thread th([]() {
		int exit_status = m_process.get_exit_status();
		std::cout << "Example 3 process returned: " << exit_status << " (" << (exit_status == 0 ? "success" : "failure") << ")" << std::endl;
		processFinished.fire();
	});
	th.detach();

	QProcess::start(program, arguments);
}

void FileProbeProcess::cancel()
{
	m_wasCanceled = false;
	m_process->kill();
}

void FileProbeProcess::onProcessFinished()
{
	m_isRunning = false;
	LOG << "probe finished with code " << exitCode();
	if (m_wasCanceled)
	{
		if (m_mustRetry)
		{
			probe(m_filename.toStdString());
		}
	}

	// Parse output
	m_streams.clear();
	//QTextStream stream(this);
	std::string line;
	do
	{
		line = "TODO";
		if (line.isEmpty())
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
	} while (!line.isNull());
	probed.fire();
}
