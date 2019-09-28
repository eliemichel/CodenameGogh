#pragma once

#include <QProcess>
#include <vector>

class FileProbeProcess : public QProcess
{
	Q_OBJECT

public:
	enum StreamType
	{
		VideoStream,
		AudioStream,
		SubtitleStream,
		DataStream,
	};

public:
	static QString locateFfprobe();

public:
	FileProbeProcess(QObject *parent = nullptr);

	// If a job is running, this will interupt it
	void probe(const QString & filename);
	void cancel();

	const std::vector<StreamType> & streams() const { return m_streams; }

signals:
	void probed();

private slots:
	void onProcessFinished();
	void readStdout();
	void readStderr();

private:
	std::vector<StreamType> m_streams;
	bool m_wasCanceled;
	bool m_isRunning;
	bool m_mustRetry;
	QString m_filename;
};
