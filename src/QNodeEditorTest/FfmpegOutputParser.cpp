#include <iostream>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "FfmpegOutputParser.h"

FfmpegOutputParser::FfmpegOutputParser()
	: re_time(R"((\d{2}):(\d{2}):(\d{2}).(\d{2}))")
	, re_duration(R"(Duration: *(\d{2}):(\d{2}):(\d{2}).(\d{2}))")
	, re_frames(R"(Frames: *(\d+))")
	, re_position_time(R"(time=(\d{2}):(\d{2}):(\d{2}).(\d{2}))")
	, re_position_frame(R"(frame= *(\d+) )")
{}

void FfmpegOutputParser::parse(const QString & out, const QString & err)
{
	// Use this to record test data
	/*
	{
		QString s;
		s.sprintf("E:/tmp/ffmpeg%03d.out", c);
		QFile file(s);
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream outStream(&file);
		outStream << out;
		file.close();
	}
	{
		QString s;
		s.sprintf("E:/tmp/ffmpeg%03d.err", c);
		QFile file(s);
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream outStream(&file);
		outStream << err;
		file.close();
	}
	++c;
	*/

	int frames = 0;
	float duration = 0.1f;
	int position_frame = 0;
	float position_time = 0;
	QRegularExpressionMatch m;
	m = re_frames.match(err);
	if (m.hasMatch()) {
		std::cout << "re_frames matched: " << m.captured(0).toStdString() << std::endl;
		frames = m.captured(1).toInt();
	}
	m = re_duration.match(err);
	if (m.hasMatch()) {
		std::cout << "re_duration matched: " << m.captured(0).toStdString() << std::endl;
		duration = m.captured(1).toInt() * 3600 + m.captured(2).toInt() * 60 + m.captured(3).toInt() + m.captured(4).toInt() * 0.01f;
	}
	if (frames > 0) {
		m = re_position_frame.match(err);
		if (m.hasMatch()) {
			std::cout << "re_position_frame matched: " << m.captured(0).toStdString() << std::endl;
			position_frame = m.captured(1).toInt();
		}
	}
	else {
		QRegularExpressionMatchIterator it = re_position_time.globalMatch(err);
		QRegularExpressionMatch m;
		while (it.hasNext()) {
			m = it.next();
		}
		if (m.hasMatch()) {
			std::cout << "re_position_time matched: " << m.captured(0).toStdString() << std::endl;
			position_time = m.captured(1).toInt() * 3600 + m.captured(2).toInt() * 60 + m.captured(3).toInt() + m.captured(4).toInt() * 0.01f;
		}
	}

	m_progress = 0;

	if (position_frame > 0) {
		m_progress = position_frame / static_cast<float>(frames) * 100;
	}

	if (position_time > 0) {
		m_progress = position_time / duration * 100;
	}
}
