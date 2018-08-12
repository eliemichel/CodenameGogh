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

#pragma once

#include <string>

/**
 * Trim whitespaces string from start (in place)
 */
void ltrim(std::string& s);

/**
 * Trim whitespaces string from end (in place)
 */
void rtrim(std::string& s);

/**
 * Trim whitespaces string in place
 */
void trim(std::string& s);

/**
 * Convert string to lower
 */
std::string toLower(const std::string & s);

bool startsWith(const std::string & s, const std::string & postfix);

bool endsWith(const std::string & s, const std::string & postfix);

bool replace(std::string & str, const std::string & from, const std::string & to);
