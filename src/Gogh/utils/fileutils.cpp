/**
 * This file is part of RayStep
 *
 * Copyright (c) 2017 -- Élie Michel <elie.michel@exppad.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * The Software is provided “as is”, without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose and non-infringement. In no event shall the
 * authors or copyright holders be liable for any claim, damages or other
 * liability, whether in an action of contract, tort or otherwise, arising
 * from, out of or in connection with the software or the use or other dealings
 * in the Software.
 */

#include "utils/fileutils.h"

#include <algorithm>
#include <string>

using namespace std;

string baseDir(const string & path) {
	size_t pos = path.find_last_of(PATH_DELIM);
	return pos != string::npos ? path.substr(0, pos) : "";
}
string shortFileName(const string & path) {
	size_t pos = path.find_last_of(PATH_DELIM);
	return pos != string::npos ? path.substr(pos + 1) : path;
}

std::string fixPath(const std::string & path) {
	std::string p = path;
	p = replaceAll(p, "/", PATH_DELIM);
	p = replaceAll(p, "\\", PATH_DELIM);
	return p;
}

bool isFile(const std::string & filename) {
	FILE *file = fopen(filename.c_str(), "r");
	if (file) {
		fclose(file);
		return true;
	}
	return false;
}
