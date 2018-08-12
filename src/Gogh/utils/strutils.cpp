/**
 * This file is part of RayStep
 *
 * Copyright (c) 2017 -- �lie Michel <elie.michel@exppad.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the �Software�), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * The Software is provided �as is�, without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose and non-infringement. In no event shall the
 * authors or copyright holders be liable for any claim, damages or other
 * liability, whether in an action of contract, tort or otherwise, arising
 * from, out of or in connection with the software or the use or other dealings
 * in the Software.
 */

#include "utils/strutils.h"

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

// Credit: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring#217605
// trim from start (in place)
void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

std::string toLower(const std::string & s) {
	std::string r = s;
	std::transform(r.begin(), r.end(), r.begin(), ::tolower);
	return r;
}

bool startsWith(const std::string & s, const std::string & prefix) {
	if (prefix.size() > s.size()) return false;
	return equal(prefix.begin(), prefix.end(), s.begin());
}

bool endsWith(const std::string & s, const std::string & postfix) {
	if (postfix.size() > s.size()) return false;
	return equal(postfix.rbegin(), postfix.rend(), s.rbegin());
}

bool replace(std::string & str, const std::string & from, const std::string & to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
