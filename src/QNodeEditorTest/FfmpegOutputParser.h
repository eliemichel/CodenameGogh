#pragma once

#include <QRegularExpression>

/**
 * Parser to read progress from raw ffmpeg output
 */
class FfmpegOutputParser {
public:
	FfmpegOutputParser();

	void parse(const QString & out, const QString & err);

	float progress() const { return m_progress; }

private:
	float m_progress;
	//int c = 0;
	QRegularExpression re_time;
	QRegularExpression re_duration;
	QRegularExpression re_frames;
	QRegularExpression re_position_time;
	QRegularExpression re_position_frame;
};
