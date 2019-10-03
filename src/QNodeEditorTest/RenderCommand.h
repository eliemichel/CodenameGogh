#pragma once

#include <QList>
#include <QStringList>
#include <memory>

class AbstractStreamData;
class AbstractCodecData;
typedef QList<std::shared_ptr<AbstractStreamData>> StreamDataList;
typedef QList<std::shared_ptr<AbstractCodecData>> CodecDataList;

class RenderCommand {
public:
	static QString locateFfmpeg();

public:
	RenderCommand(const QString & filename,
	              const StreamDataList & streams,
	              const CodecDataList & codecs);

	const QString & program() { return m_program; }
	const QStringList & arguments() { return m_arguments; }

private:
	static QStringList buildArguments(const QString & filename,
	                                  const StreamDataList & streams,
	                                  const CodecDataList & codecs);

private:
	const QString m_program;
	const QStringList m_arguments;
};
