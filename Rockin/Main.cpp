#include <memory>
#include "Sim.h"
#include <time.h>
#include "CoreException.h"
#include "Logger.h"
#include "DataIO.h"
#include <string>
#include <conio.h>

/* Rockin' - Created by Scott Emerson

Description

A simple 2D game / simulation prototyping engine. Designed
to be data-driven and (eventually) simple to use at the
expense of scalability. Game objects (CoreEntities), scenes,
and behaviors are all defined by data files and script files.
(Lame project name comes from the Asteroids-like test game
that I've been using to ensure that features are working.)

Libraries

This project uses the following additional libraries:
SFML v2.4.2, (c) 2007 - 2019 Laurent Gomila (https://www.sfml-dev.org/)

AngelScript v2.33.0, (c) 2003 - 2019 Andreas Jonsson
(https://www.angelcode.com/angelscript/)

sdelib v1.0 - A basic boilerplate library of foundational classes
that I wrote for use in this project and others.
(https://github.com/sdemerson42/SdeLib)
*/

/*
main() args:
-logfile <filename> : Specify the name of the logfile produced during runtime.
Default is "RuntimeLog.txt"
-console : Send all log messages to console in addition to log file.
-nologfile : Don't send log messages to a log file.
*/

int main(int argc, char *argv[])
{
	const float version = 0.064f;

	// Temporary display window constraints
	const int windowWidth = 800;
	const int windowHeight = 600;

	try
	{
		// Process valid arguments. Bad arguments are ignored for now.
		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
			{
				auto arg{ std::string{ argv[i] } };
				if (arg == "-logfile")
				{
					if (++i < argc)
					{
						Core::Logger::setFileName(argv[i]);
					}
					else break;
				}

				if (arg == "-console")
				{
					Core::Logger::setLogToConsole(true);
				}

				if (arg == "-nologfile")
				{
					Core::Logger::setFileName("");
				}
			}
		}
	}
	catch (const Core::CoreException &e)
	{
		std::cerr << e.what();
		return e.errCode;
	}

	try
	{
		Core::Logger::initialize();
		Core::Logger::log("-----------Rockin' Version " + std::to_string(version) + " -----------\nStarting execution.");

		// To-do: Replace temporary RNG with something more robust.
		srand(std::time(NULL));

		// Construct DataIO regular expressions
		Core::Tokenizer::initialize();

		std::unique_ptr<Core::Sim> sim = std::make_unique<Core::Sim>(windowWidth, windowHeight, "Rockin'");
		sim->execute();

		Core::Logger::log("Completing execution.");
	}
	catch(const Core::CoreException &e)
	{
		if (e.errCode != 1)
		{
			Core::Logger::log("Error code: " + std::to_string(e.errCode) + '\n' + e.what());
			Core::Logger::log("Aborting simulation.");
		}
		else
		{
			std::cerr << "Error code: " << e.errCode << '\n';
			std::cerr << e.what();
		}
		return e.errCode;
	}
	catch (const std::exception &e)
	{
		Core::Logger::log(e.what());
		Core::Logger::log("Aborting simulation.");
	}

	_getch();
	return 0;
}