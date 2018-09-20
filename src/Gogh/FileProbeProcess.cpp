#include "FileProbeProcess.h"
#include "Logger.h"

#include <QTextStream>

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

void FileProbeProcess::probe(std::string filename)
{
	m_filename = QString::fromStdString(filename);
	if (m_isRunning)
	{
		m_mustRetry = true;
		cancel();
	}

	QString program = FileProbeProcess::locateFfprobe();
	QStringList arguments = QStringList()
		<< "-v" << "error"
		<< "-show_entries" << "stream=codec_type"
		<< "-print_format" << "default=noprint_wrappers=1:nokey=1"
		<< m_filename;

	DEBUG_LOG << program.toStdString() << " " << arguments.join(" ").toStdString();

	m_wasCanceled = false;
	m_isRunning = true;
	m_mustRetry = false;

	QProcess::start(program, arguments);
}

void FileProbeProcess::cancel()
{
	m_wasCanceled = false;
	kill();
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
	probed.fire();
}

void FileProbeProcess::readStdout()
{}

void FileProbeProcess::readStderr()
{
	WARN_LOG << readAllStandardError().toStdString();
}
