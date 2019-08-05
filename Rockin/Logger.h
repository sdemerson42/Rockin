#pragma once

#include <string>

namespace Core
{

	/*
	Logger - Collection of static members / methods allowing simple log
	files to be created during runtime.
	*/

	class Logger
	{
	public:
		static void initialize();
		static void log(const std::string &message);
		// Static accessors
		static void setLogToConsole(bool val)
		{
			m_logToConsole = val;
		}
		static void setFileName(const std::string &fname)
		{
			m_logFileName = fname;
		}
	private:
		Logger()
		{}
		static std::string m_logFileName;
		static bool m_logToConsole;
	};
}
