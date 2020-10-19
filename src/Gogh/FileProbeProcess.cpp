#include <iostream>

#include <QTextStream>

#include "FileProbeProcess.h"

QString FileProbeProcess::locateFfprobe()
{
	return "ffprobe";
}

FileProbeProcess::FileProbeProcess(QObject *parent)
	: QProcess(parent)
	, m_wasCanceled(true)
	, m_isRunning(false)
	, m_mustRetry(false)
{
	connect(this, QOverload<int>::of(&QProcess::finished), this, &FileProbeProcess::onProcessFinished);
	connect(this, &QProcess::readyReadStandardOutput, this, &FileProbeProcess::readStdout);
	connect(this, &QProcess::readyReadStandardError, this, &FileProbeProcess::readStderr);

	setReadChannel(StandardOutput);
}

void FileProbeProcess::probe(const QString & filename)
{
	m_filename = filename;
	if (m_isRunning)
	{
		m_mustRetry = true;
		cancel();
		return;
	}

	QString program = FileProbeProcess::locateFfprobe();
	QStringList arguments = QStringList()
		<< "-v" << "error"
		<< "-show_entries" << "stream=codec_type"
		<< "-print_format" << "default=noprint_wrappers=1:nokey=1"
		<< filename;

	std::cout << program.toStdString() << " " << arguments.join(" ").toStdString() << std::endl;

	m_wasCanceled = false;
	m_isRunning = true;
	m_mustRetry = false;

	QProcess::start(program, arguments);
}

void FileProbeProcess::cancel()
{
	m_wasCanceled = true;
	kill();
}

void FileProbeProcess::onProcessFinished()
{
	m_isRunning = false;
	std::cout << "probe finished with code " << exitCode() << std::endl;
	if (m_wasCanceled)
	{
		if (m_mustRetry)
		{
			probe(m_filename);
		}
		return;
	}

	// Parse output
	m_streams.clear();
	QTextStream stream(this);
	QString line;
	do
	{
		line = stream.readLine();
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
	emit probed();
}

void FileProbeProcess::readStdout()
{
}

void FileProbeProcess::readStderr()
{
	std::cerr << readAllStandardError().toStdString() << std::endl;
}
