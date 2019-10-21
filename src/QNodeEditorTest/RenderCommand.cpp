#include <QMap>
#include <QFileInfo>

#include "RenderCommand.h"

#include "nodes/AbstractStreamData.h"
#include "nodes/AbstractCodecData.h"

QString RenderCommand::locateFfmpeg() {
	return "ffmpeg";
}

RenderCommand::RenderCommand(const QString & filename,
	                         const StreamDataList & streams,
	                         const CodecDataList & codecs)
	: m_program(locateFfmpeg())
	, m_arguments(buildArguments(filename, streams, codecs))
	, m_outputFilename(filename)
{}

QString RenderCommand::displayOutputFile()
{
	return QFileInfo(m_outputFilename).fileName();
}

QStringList RenderCommand::buildArguments(const QString & filename,
	                                      const StreamDataList & streams,
	                                      const CodecDataList & codecs)
{
	StreamDataList::const_iterator sit;
	CodecDataList::const_iterator cit;

	Q_ASSERT(streams.size() == codecs.size());
	sit = streams.constBegin();
	cit = codecs.constBegin();
	for ( ; sit != streams.constEnd() ; ++sit,++cit) {
		const auto & s = *sit;
		const auto & c = *cit;
		Q_ASSERT(
			(s->type().id == "video"    && c->type().id == "video_codec"   ) ||
			(s->type().id == "audio"    && c->type().id == "audio_codec"   ) ||
			(s->type().id == "subtitle" && c->type().id == "subtitle_codec") ||
			(s->type().id == "data"     && c->type().id == "data_codec"    )
		);
	}

	int streamCount = streams.size();
	QVector<int> streamsFileIndex(streamCount); // index of the stream's origin file (different from stream index if several streams come from the same input)
	QVector<int> streamsIndexAmongType(streamCount); // stream 'i' is the streamsIndexAmongType[i]th stream of its type (video, audio, etc.)
	QMap<QString, int> streamCountPerType;

	QList<std::shared_ptr<AbstractStreamData>> inputFiles;
	QMap<QString, int> inputFilenamesMap;

	int i = 0;
	for (const auto & s : streams) {
		const QString & filename = s->filename();
		if (!inputFilenamesMap.contains(filename)) {
			inputFilenamesMap[filename] = inputFilenamesMap.count();
			inputFiles << s;
		}
		streamsFileIndex[i] = inputFilenamesMap[filename];

		const QString & sname = s->ffmpegShortName();
		if (!streamCountPerType.contains(sname)) streamCountPerType[sname] = 0;
		streamsIndexAmongType[i] = streamCountPerType[sname];
		++streamCountPerType[sname];
		++i;
	}

	QStringList cmd;
	// Input files demuxing
	for (const auto & f : inputFiles) {
		cmd += f->options();
		cmd << "-i" << f->filename();
	}
	i = 0;
	// Stream mapping
	for (const auto & s : streams) {
		cmd << "-map" << (QString::number(streamsFileIndex[i]) + ":" + QString::number(s->streamId()));
		++i;
	}
	// Output codecs
	sit = streams.constBegin();
	cit = codecs.constBegin();
	i = 0;
	for (; sit != streams.constEnd() ; ++sit,++cit) {
		const auto & s = *sit;
		const auto & c = *cit;
		const QString & sname = s->ffmpegShortName();
		cmd << ("-c:" + sname + ":" + QString::number(streamsIndexAmongType[i]));
		cmd << c->name();
		cmd += c->options();
		++i;
	}
	// Output file muxing
	cmd << filename;
	return cmd;
}
