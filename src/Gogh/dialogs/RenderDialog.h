#pragma once

#include <QDialog>
#include <QStringList>
#include <QElapsedTimer>
#include <QTimer>

#include "RenderCommand.h"
#include "FfmpegOutputParser.h"

class QProcess;

namespace Ui {
	class RenderDialog;
}

class RenderDialog : public QDialog
{
	Q_OBJECT
public:
	RenderDialog(const RenderCommand & cmd, QWidget *parent = nullptr);
	~RenderDialog();

protected:
	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	void setRunning(bool running);
	void setProgress(float progress);

private slots:
	void onProcessFinished();
	void readStdout();
	void readStderr();
	void cancel();

	/**
	* Parse ffmpeg output to extract the progress or potential errors.
	* Do not call this directly too often, prefer using needParsing for a rate
	* limited version.
	*/
	void parse();

	/**
	* Call output parsing, unless it has already been called very recently. In
	* the latter case, schedule the parsing for later.
	*/
	void needParsing();

private:
	Ui::RenderDialog *ui;
	bool m_isRunning;
	RenderCommand m_cmd;
	QProcess *m_ffmpegProcess;
	QString m_processStdout;
	QString m_processStderr;

	FfmpegOutputParser m_parser;
	qint64 m_minimumParseInterval; // in milliseconds
	QElapsedTimer m_lastParseTimer;
	QTimer m_nextParseTimer;
};
