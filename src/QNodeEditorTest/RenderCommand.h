#pragma once

#include <QList>
#include <QStringList>
#include <memory>

class AbstractStreamData;
typedef QList<std::shared_ptr<AbstractStreamData>> StreamDataList;

class RenderCommand {
public:
	static QString locateFfmpeg();

public:
	RenderCommand(const QString & filename, const StreamDataList & streams);

	const QString & program() { return m_program; }
	const QStringList & arguments() { return m_arguments; }

private:
	static QStringList buildArguments(const QString & filename, const StreamDataList & streams);

private:
	const QString m_program;
	const QStringList m_arguments;
};
