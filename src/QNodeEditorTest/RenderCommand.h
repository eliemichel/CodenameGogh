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

	/**
	 * Short name of the output file, used for display only
	 */
	QString displayOutputFile() const;

	/**
	 * Return true if the file that the render command is supposed to write on
	 * already exists.
	 */
	bool fileExists() const;

private:
	static QStringList buildArguments(const QString & filename,
	                                  const StreamDataList & streams,
	                                  const CodecDataList & codecs);

private:
	const QString m_program;
	const QStringList m_arguments;
	const QString m_outputFilename;
};
