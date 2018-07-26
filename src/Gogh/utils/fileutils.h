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

#pragma once

#include "strutils.h"

#include <string>
#include <sstream>

#ifdef _WIN32
constexpr auto PATH_DELIM = "\\";
constexpr auto PATH_DELIM_CHAR = '\\';
#else
constexpr auto PATH_DELIM = "/";
constexpr auto PATH_DELIM_CHAR = '/';
#endif

inline std::string joinPath(const std::string& head) {
	return head;
}

template<typename... Args>
inline std::string joinPath(const std::string& head, Args&&... rest) {
	std::string correctedHead = endsWith(head, PATH_DELIM) ? head.substr(0, head.size() - 1) : head;
	return correctedHead + std::string(PATH_DELIM) + joinPath(rest...);
}

std::string baseDir(const std::string & path);
std::string shortFileName(const std::string & path);

// Transform both all / and \ into PATH_DELIM
std::string fixPath(const std::string & path);

bool isFile(const std::string & filename);





