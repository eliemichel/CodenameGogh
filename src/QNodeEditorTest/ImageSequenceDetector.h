#pragma once

#include <QString>

/**
 * Fully static class providing utility functions to detect image sequences
 */
class ImageSequenceDetector {
public:
	static QString autodetect(const QString & rawFilename);
};
