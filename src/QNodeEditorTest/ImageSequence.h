#pragma once

#include <QString>

/**
 * Fully static class providing utility functions to detect image sequences
 */
class ImageSequence {
public:
	bool isValid() const;

	/**
	 * Get actual filename for a given frame.
	 * Frame index 0 queries file with number startFrame.
	 */
	QString getFrameFilename(int frame = 0) const;

public:
	static ImageSequence fromAutodetect(const QString & rawFilename);

public:
	QString basename;
	int startFrame;
	int endFrame;
};
