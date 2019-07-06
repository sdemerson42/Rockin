#include <memory>
#include "Sim.h"
#include <time.h>
#include <exception>
#include "Logger.h"

/* Rockin'
A simple 2D game / simulation prototyping engine. Designed
to be data-driven and (eventually) simple to use at the
expense of scalability.
*/

int main()
{
	const float version = .0631f;
	try
	{
		Logger::log("-----------Rockin' Version " + std::to_string(version) + " -----------\nStarting execution.");

		// To-do: Replace temporary RNG with something more robust.
		srand(std::time(NULL));

		std::unique_ptr<Core::Sim> sim = std::make_unique<Core::Sim>(800, 600, "Rockin'");
		sim->execute();

		Logger::log("Completing execution.");
	}
	catch(const std::exception &e)
	{
		Logger::log(e.what());
		Logger::log("Aborting simulation.");
	}

	return 0;
}