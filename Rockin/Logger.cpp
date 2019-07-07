#include "Logger.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <time.h>
#include "CoreException.h"

namespace Core
{

	std::string Logger::m_logFileName{ "RuntimeLog.txt" };
	bool Logger::m_logToConsole{ false };

	void Logger::log(const std::string &message)
	{
		std::ofstream ofs{ m_logFileName, std::ios::app };
		if (!ofs)
		{
			CoreException e{ "Bad logfile.", 1 };
			throw(e);
		}

		auto timePoint = std::chrono::system_clock::now();
		auto currentTime = std::chrono::system_clock::to_time_t(timePoint);
		char cstrTime[80];

		ctime_s(cstrTime, sizeof(cstrTime), &currentTime);

		std::string outString{ cstrTime + message + "\n\n" };

		ofs << outString;
		if (m_logToConsole)
		{
			std::cout << outString;
		}
	}
}