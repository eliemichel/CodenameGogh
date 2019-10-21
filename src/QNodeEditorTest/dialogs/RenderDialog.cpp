#include <iostream>

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QFontDatabase>
#include <QCloseEvent>
#include <QProcess>
#include <QScrollBar>
#include <QMessageBox>
#include <QTimer>

#include "Style.h"
#include "RenderDialog.h"
#include "ui_RenderDialog.h"

RenderDialog::RenderDialog(const RenderCommand & cmd, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::RenderDialog())
	, m_cmd(cmd)
	, m_isRunning(false)
	, m_ffmpegProcess(nullptr)
	, m_minimumParseInterval(1000)
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	int id = QFontDatabase::addApplicationFont(":/fonts/cmunbmr.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	setFont(QFont(family));
	setStyleSheet(Gogh::Style::dialogStyle());

	ui->setupUi(this);
	
	ui->processOutput->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
	ui->processOutput->setText("");

	connect(ui->cancelButton, &QPushButton::clicked, this, &RenderDialog::cancel);
	connect(ui->cancelAllButton, &QPushButton::clicked, this, &RenderDialog::cancel);
	connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
	connect(ui->titleBarCloseButton, &QPushButton::clicked, this, &QDialog::close);

	m_nextParseTimer.setSingleShot(true);
	connect(&m_nextParseTimer, &QTimer::timeout, this, &RenderDialog::parse);

	m_lastParseTimer.invalidate();
	
	setRunning(false);
}

RenderDialog::~RenderDialog()
{
	if (m_ffmpegProcess) {
		delete m_ffmpegProcess;
		m_ffmpegProcess = nullptr;
	}
}

void RenderDialog::closeEvent(QCloseEvent *event)
{
	if (m_isRunning)
	{
		event->ignore();
	}
	else
	{
		QDialog::closeEvent(event);
	}
}

void RenderDialog::showEvent(QShowEvent *event)
{
	if (m_isRunning) {
		return;
	}

	if (m_ffmpegProcess) {
		delete m_ffmpegProcess;
	}
	m_ffmpegProcess = new QProcess(this);
	connect(m_ffmpegProcess, QOverload<int>::of(&QProcess::finished), this, &RenderDialog::onProcessFinished);
	connect(m_ffmpegProcess, &QProcess::readyReadStandardOutput, this, &RenderDialog::readStdout);
	connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RenderDialog::readStderr);

	// Prompts the command sent to ffmpeg right before

	std::cout << m_cmd.program().toStdString() << " " << m_cmd.arguments().join(" ").toStdString() << std::endl;

	m_ffmpegProcess->start(m_cmd.program(), m_cmd.arguments());

	m_processStdout = "";
	m_processStderr = "";
	m_lastParseTimer.invalidate();
	m_nextParseTimer.stop();
	setProgress(0);

	setRunning(m_ffmpegProcess->isOpen());

	QDialog::showEvent(event);
}

void RenderDialog::setRunning(bool running)
{
	m_isRunning = running;
	ui->closeButton->setEnabled(!running);
	ui->titleBarCloseButton->setEnabled(!running);
	ui->cancelAllButton->setEnabled(running);

	if (m_isRunning) {
		ui->subtitle->setText("<span style=\"font-size:12pt;\">Exporting " + m_cmd.displayOutputFile() + "...</span>");
	}
}

void RenderDialog::setProgress(float progress)
{
	int percent = static_cast<int>(progress);
	ui->progressBar->setValue(percent);
	ui->progressNumber->setText(QString::number(percent) + "%");
}

void RenderDialog::parse()
{
	m_parser.parse(m_processStdout, m_processStderr);

	if (m_isRunning) {
		setProgress(m_parser.progress());
	}

	m_lastParseTimer.start();
}

void RenderDialog::needParsing()
{
	if (m_lastParseTimer.isValid() && m_lastParseTimer.elapsed() < m_minimumParseInterval) {
		if (!m_nextParseTimer.isActive()) {
			m_nextParseTimer.start(m_minimumParseInterval - m_lastParseTimer.elapsed());
		}
	}
	else {
		parse();
	}
}

void RenderDialog::onProcessFinished()
{
	std::cout << "render finished with code " << m_ffmpegProcess->exitCode() << std::endl;
	setRunning(false);
	setProgress(100);
	ui->outputScrollArea->verticalScrollBar()->setValue(ui->outputScrollArea->verticalScrollBar()->maximum());
}

void RenderDialog::readStdout()
{
	m_processStdout += m_ffmpegProcess->readAllStandardOutput();
	needParsing();

	ui->processOutput->setText(m_processStdout);
	ui->outputScrollArea->verticalScrollBar()->setValue(ui->outputScrollArea->verticalScrollBar()->maximum());
}

void RenderDialog::readStderr()
{
	m_processStderr += m_ffmpegProcess->readAllStandardError();
	needParsing();

	ui->processOutput->setText(m_processStderr);
	ui->outputScrollArea->verticalScrollBar()->setValue(ui->outputScrollArea->verticalScrollBar()->maximum());
}

void RenderDialog::cancel()
{
	if (!m_ffmpegProcess)
	{
		return;
	}

	QMessageBox confirmationDialog;
	confirmationDialog.setText("Cancel render");
	confirmationDialog.setInformativeText("Do you want to kill the current render job?");
	confirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	confirmationDialog.setDefaultButton(QMessageBox::No);
	if (confirmationDialog.exec() == QMessageBox::No)
	{
		return;
	}

	// TODO: try to terminate() first, if not on windows
	m_ffmpegProcess->kill();
}
