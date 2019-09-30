#pragma once

#include <QList>
#include <QStringList>
#include <memory>
#include "nodes/CodecData.h"

class AbstractStreamData;
typedef QList<std::shared_ptr<AbstractStreamData>> StreamDataList;

class RenderCommand {
public:
	static QString locateFfmpeg();

public:
	RenderCommand(const QString & filename,
	              const StreamDataList & streams,
	              const CodecData & videoCodec);

	const QString & program() { return m_program; }
	const QStringList & arguments() { return m_arguments; }

private:
	static QStringList buildArguments(const QString & filename,
	                                  const StreamDataList & streams,
	                                  const CodecData & videoCodec);

private:
	const QString m_program;
	const QStringList m_arguments;
};
