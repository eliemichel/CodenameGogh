#ifndef H_FILEPROBEPROCESS
#define H_FILEPROBEPROCESS

#include <QProcess>
#include <vector>
#include <string>
#include <node.h>
#include "Signal.h"

class FileProbeProcess : public QProcess
{
	Q_OBJECT

public:
	static QString locateFfprobe();

public:
	FileProbeProcess(QObject *parent = nullptr);

	// If a job is running, this will interupt it
	void probe(std::string filename);
	void cancel();

	const std::vector<StreamType> & streams() const { return m_streams; }

public: // signals
	Signal<> probed;

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
