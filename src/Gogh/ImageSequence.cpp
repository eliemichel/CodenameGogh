#include <iostream>

#include <QFileInfo>
#include <QRegularExpression>
#include <QDir>

#include "ImageSequence.h"

bool ImageSequence::isValid() const
{
	return basename != "";
}

QString ImageSequence::getFrameFilename(int frame) const
{
	QString str;
	str.sprintf(basename.toStdString().data(), startFrame + frame);
	return str;
}

ImageSequence ImageSequence::fromAutodetect(const QString & rawFilename)
{
	QRegularExpression isImageFile(R"((\.png|\.jpg|\.exr)$)");
	QRegularExpression digitSequence(R"((\d+)\.\w+$)");

	if (!isImageFile.match(rawFilename).hasMatch()) {
		return ImageSequence();
	}

	QFileInfo info(rawFilename);
	if (!info.isFile()) {
		return ImageSequence();
	}

	const QString & shortName = info.fileName();
	QRegularExpressionMatch m = digitSequence.match(shortName);
	if (!m.hasMatch()) {
		return ImageSequence();
	}

	int nDigits = m.captured(1).length();
	int frame = m.captured(1).toInt();
	QString before = shortName.left(m.capturedStart(1));
	QString after = shortName.mid(m.capturedEnd(1));

	QFileInfo f(info);
	int startFrame = frame;
	while (f.isFile()) {
		// try %0nd
		// TODO: try other patterns, like %nd (no leading 0)
		--startFrame;
		QString seq = QStringLiteral("%1").arg(startFrame, nDigits, 10, QChar('0'));
		f.setFile(info.dir(), before + seq + after);
	}
	++startFrame;

	f.setFile(info.filePath());
	int endFrame = frame;
	while (f.isFile()) {
		// try %0nd
		// TODO: try other patterns, like %nd (no leading 0)
		++endFrame;
		QString seq = QStringLiteral("%1").arg(endFrame, nDigits, 10, QChar('0'));
		f.setFile(info.dir(), before + seq + after);
	}
	--endFrame;

	QString pattern = before + "%0" + QString::number(nDigits) + "d" + after;
	QFileInfo newInfo(info.dir(), pattern);

	std::cout << "Found image sequence pattern '" << pattern.toStdString() << "' from " << startFrame << " to " << endFrame << std::endl;

	ImageSequence seq;
	seq.basename = newInfo.filePath();
	seq.startFrame = startFrame;
	seq.endFrame = endFrame;
	return seq;
}
