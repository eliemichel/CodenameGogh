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

#include "Logger.h"
#include "utils/fileutils.h"
#include <chrono>
#include <ctime>
#include <cstring>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

size_t Logger::align_width = 0;

bool EnableVTMode()
{
#ifdef _WIN32
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return false;
	}
#endif
	return true;
}

Logger::Logger(const char *func, const char *file, int line, Logger::Level level)
{
	if (Logger::align_width == 0) {
		Logger::init();
		Logger::align_width = 1;
	}

    {
        using namespace std::chrono;
        std::time_t now_c = system_clock::to_time_t(system_clock::now());
#ifdef _WIN32
		struct tm t;
		struct tm * timeinfo = &t;
		errno_t err = localtime_s(timeinfo, &now_c);
#else // _WIN32
		struct tm * timeinfo = std::localtime(&now_c);
#endif // _WIN32
		char buffer[26];
		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", timeinfo);
		m_ss << buffer << ": ";
    }

    switch(level)
    {
    case Logger::LDEBUG:
        m_ss << Color::bold_grey   << "DEBUG  " << Color::nocolor;
        break;
    case Logger::LVERBOSE:
        m_ss << Color::bold_white  << "VERBOSE" << Color::nocolor;
        break;
    case Logger::LINFO:
        m_ss << Color::bold_white  << "INFO   " << Color::nocolor;
        break;
    case Logger::LWARNING:
        m_ss << Color::bold_yellow << "WARNING" << Color::nocolor;
        break;
    case Logger::LERROR:
        m_ss << Color::bold_red    << "ERROR  " << Color::nocolor;
        break;
    }

    if( level == LDEBUG )
    {
        std::stringstream pos;
        pos << " (" << func << "():" << Logger::shorterFilename(file) << ":" << line << ")";
        Logger::align(pos);
        m_ss << Color::blue << pos.str() << Color::nocolor;
    }

    m_ss << " ";

    switch( level) {
    case Logger::LDEBUG:   m_ss << Color::bold_grey;    break;
    case Logger::LVERBOSE: m_ss << Color::nocolor; break;
    case Logger::LINFO:    m_ss << Color::nocolor; break;
    case Logger::LWARNING: m_ss << Color::yellow;  break;
    case Logger::LERROR:   m_ss << Color::red;     break;
    }
}

Logger::~Logger()
{
    m_ss << Color::nocolor;
    std::string str = m_ss.str();
    // trim extra newlines
    while ( str.empty() == false && str[str.length() - 1] == '\n')
        str.resize(str.length() - 1);
    std::cerr << str << std::endl;
}

const char * Logger::shorterFilename(const char *filename)
{
    const char *last_slash  = std::strrchr(filename, PATH_DELIM_CHAR);
    if ( last_slash == 0)
        return filename;
    while ( last_slash > filename && last_slash[-1] != PATH_DELIM_CHAR)
        --last_slash;
    return last_slash;
}

void Logger::align(std::stringstream &ss)
{
	size_t l = ss.str().length();
    Logger::align_width = max(Logger::align_width, l);
	size_t x = Logger::align_width;  // why is that needed?
	ss << std::setw(x - l) << "";
}

void Logger::init() {
	if (!EnableVTMode()) {
		std::cerr << "Warning: Logger could not enable Virtual Terminal mode." << std::endl;
	}
}
