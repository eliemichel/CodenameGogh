#include "RenderProcess.h"
#include "Logger.h"

#include <iostream>

std::string RenderProcess::locateFfmpeg()
{
	return "ffmpeg";
}

RenderProcess::RenderProcess(const std::vector<std::string> & args, QObject *parent)
	: QProcess(parent)
	, m_args(args)
{
	connect(this, QOverload<int>::of(&QProcess::finished), this, &RenderProcess::onProcessFinished);
	connect(this, &QProcess::readyReadStandardOutput, this, &RenderProcess::readStdout);
	connect(this, &QProcess::readyReadStandardError, this, &RenderProcess::readStderr);

	setInputChannelMode(ForwardedInputChannel);
}

void RenderProcess::start()
{
	QString program = QString::fromStdString(RenderProcess::locateFfmpeg());
	QStringList arguments;
	for (const std::string & arg : m_args)
	{
		arguments << QString::fromStdString(arg);
	}

	//Prompts the command sent to ffmpeg right before
	DEBUG_LOG << program.toStdString() << " " << arguments.join(" ").toStdString();

	QProcess::start(program, arguments);
}

void RenderProcess::onProcessFinished()
{
	LOG << "render finished with code " << exitCode();
}

void RenderProcess::readStdout()
{
	std::cout << readAllStandardOutput().toStdString() << std::flush;
}

void RenderProcess::readStderr()
{
	std::cerr << readAllStandardError().toStdString() << std::flush;
}

