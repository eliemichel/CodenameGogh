#include "RenderDialog.h"

#include "Logger.h"

#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QCloseEvent>
#include <QPushButton>
#include <QMessageBox>

#include <QFontDatabase>
#include <QtGlobal>
#include <QProcess>

std::string RenderDialog::locateFfmpeg()
{
	return "E:/Program Files/ffmpeg/bin/ffmpeg";
}

RenderDialog::RenderDialog(std::string cmd, QWidget *parent)
	: QDialog(parent)
	, m_cmd(cmd)
	, m_isRunning(false)
	, m_ffmpegProcess(nullptr)
{
	QVBoxLayout *layout = new QVBoxLayout();

	m_processOutputLabel = new QLabel();
	m_processOutputLabel->setWordWrap(true);
	m_processOutputLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

	m_scrollArea = new QScrollArea();
	m_scrollArea->setWidget(m_processOutputLabel);
	m_scrollArea->setWidgetResizable(true);
	layout->addWidget(m_scrollArea);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	m_cancelButton = new QPushButton("Cancel");
	connect(m_cancelButton, &QPushButton::clicked, this, &RenderDialog::cancel);
	buttonLayout->addWidget(m_cancelButton);
	m_closeButton = new QPushButton("Close");
	connect(m_closeButton, &QPushButton::clicked, this, &QDialog::close);
	buttonLayout->addWidget(m_closeButton);
	layout->addLayout(buttonLayout);

	this->setLayout(layout);
	this->setMinimumSize(600, 300);

	setRunning(false);
}

RenderDialog::~RenderDialog()
{
	if (m_ffmpegProcess)
	{
		delete m_ffmpegProcess;
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
	if (m_isRunning)
	{
		return;
	}

	QString program = QString::fromStdString(RenderDialog::locateFfmpeg());
	QStringList arguments = QString::fromStdString(m_cmd).split(" ");

	if (m_ffmpegProcess)
	{
		delete m_ffmpegProcess;
	}
	m_ffmpegProcess = new QProcess(this);
	connect(m_ffmpegProcess, QOverload<int>::of(&QProcess::finished), this, &RenderDialog::onProcessFinished);
	connect(m_ffmpegProcess, &QProcess::readyReadStandardOutput, this, &RenderDialog::readStdout);
	connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RenderDialog::readStderr);

	setRunning(true);

	m_ffmpegProcess->start(program, arguments);

	QDialog::showEvent(event);
}

void RenderDialog::setRunning(bool running)
{
	m_isRunning = running;
	m_closeButton->setEnabled(!running);
	m_cancelButton->setEnabled(running);
}

void RenderDialog::onProcessFinished()
{
	DEBUG_LOG << "render finished with code " << m_ffmpegProcess->exitCode();
	setRunning(false);
	m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
}

void RenderDialog::readStdout()
{
	m_processOutputLabel->setText(m_processOutputLabel->text() + m_ffmpegProcess->readAllStandardOutput());
	m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
}

void RenderDialog::readStderr()
{
	m_processOutputLabel->setText(m_processOutputLabel->text() + m_ffmpegProcess->readAllStandardError());
	m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
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

