#include <QMap>

#include "RenderCommand.h"
#include "nodes/VideoStreamData.h"
#include "nodes/AudioStreamData.h"
#include "nodes/SubtitleStreamData.h"
#include "nodes/DataStreamData.h"

QString RenderCommand::locateFfmpeg() {
	return "ffmpeg";
}

RenderCommand::RenderCommand(const QString & filename, const StreamDataList & streams)
	: m_program(locateFfmpeg())
	, m_arguments(buildArguments(filename, streams))
{}

QStringList RenderCommand::buildArguments(const QString & filename, const StreamDataList & streams) {
	int streamCount = streams.size();
	QVector<int> streamsFileIndex(streamCount); // index of the stream's origin file (different from stream index if several streams come from the same input)
	QVector<int> streamsIndexAmongType(streamCount); // stream 'i' is the streamsIndexAmongType[i]th stream of its type (video, audio, etc.)
	QMap<QString, int> streamCountPerType;

	QList<QString> inputFilenames;
	QMap<QString, int> inputFilenamesMap;

	int i = 0;
	for (const auto & s : streams) {
		const QString & filename = s->filename();
		if (!inputFilenamesMap.contains(filename)) {
			inputFilenamesMap[filename] = inputFilenamesMap.count();
			inputFilenames << filename;
		}
		streamsFileIndex[i] = inputFilenamesMap[filename];

		const QString & sname = s->ffmpegShortName();
		if (!streamCountPerType.contains(sname)) streamCountPerType[sname] = 0;
		streamsIndexAmongType[i] = streamCountPerType[sname];
		++streamCountPerType[sname];
		++i;
	}

	QStringList cmd;
	for (const auto & f : inputFilenames) {
		// TODO: input options
		cmd << "-i" << f;
	}
	i = 0;
	for (const auto & s : streams) {
		cmd << "-map" << (QString::number(streamsFileIndex[i]) + ":" + QString::number(s->streamId()));
		++i;
	}
	i = 0;
	for (const auto & s : streams) {
		const QString & sname = s->ffmpegShortName();
		cmd << ("-c:" + sname + ":" + QString::number(streamsIndexAmongType[i])) << "copy";
		++i;
	}
	// TODO: output options
	cmd << filename;
	return cmd;
}
