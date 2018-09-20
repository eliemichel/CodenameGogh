#ifndef H_FILEPROBEPROCESS
#define H_FILEPROBEPROCESS

#include <QProcess>
#include <vector>
#include <node.h>

class FileProbeProcess : public QProcess
{
	Q_OBJECT

public:
	static QString locateFfprobe();

public:
	FileProbeProcess(QObject *parent = nullptr);

	// If a job is running, this will interupt it
	void probe(QString filename);
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

#endif // H_FILEPROBEPROCESS
