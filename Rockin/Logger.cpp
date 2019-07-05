#include "Logger.h"
#include <fstream>
#include <chrono>
#include <time.h>
#include <exception>

// To-do: Allow modification of m_logFileName
std::string Logger::m_logFileName{ "RuntimeLog.txt" };

void Logger::log(const std::string &message)
{
	std::ofstream ofs{ m_logFileName, std::ios::app };
	if (!ofs)
	{
		std::exception e{ "Failed to open log output file." };
		throw(e);
	}

	auto timePoint = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::to_time_t(timePoint);
	char cstrTime[80];

	ctime_s(cstrTime, sizeof(cstrTime), &currentTime);

	ofs << cstrTime << message << "\n\n";
}