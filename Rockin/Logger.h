#pragma once

#include <string>

/*
Logger - Collection of static members / methods allowing simple log
files to be created during runtime.
*/

class Logger
{
public:
	static void log(const std::string &message);
private:
	Logger()
	{}
	static std::string m_logFileName;
};
