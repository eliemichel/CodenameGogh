#ifndef H_RENDERDIALOG
#define H_RENDERDIALOG

#include <QDialog>
#include <string>

class QLabel;
class QScrollArea;
class QProcess;

class RenderDialog : public QDialog
{
	Q_OBJECT
public:
	static std::string locateFfmpeg();

public:
	RenderDialog(std::string cmd, QWidget *parent = nullptr);
	~RenderDialog();

protected:
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	void setRunning(bool running);

private slots:
	void onProcessFinished();
	void readStdout();
	void readStderr();

private:
	bool m_isRunning;
	std::string m_cmd;
	QProcess *m_ffmpegProcess;
	QScrollArea *m_scrollArea;
	QLabel *m_processOutputLabel;
	QPushButton *m_closeButton;
};

#endif // H_RENDERDIALOG
