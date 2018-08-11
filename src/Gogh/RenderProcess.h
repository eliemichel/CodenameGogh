#ifndef H_RENDERPROCESS
#define H_RENDERPROCESS

#include <QProcess>

class RenderProcess : public QProcess
{
	Q_OBJECT
public:
	static std::string locateFfmpeg();

public:
	RenderProcess(const std::vector<std::string> & args, QObject *parent = nullptr);

	void start();

private slots:
	void onProcessFinished();
	void readStdout();
	void readStderr();

private:
	std::vector<std::string> m_args;
};

#endif // H_RENDERPROCESS
