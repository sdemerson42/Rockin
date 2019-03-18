#include <memory>
#include "Sim.h"
#include <time.h>

int main()
{
	srand(std::time(NULL));

	std::unique_ptr<Core::Sim> sim{ new Core::Sim };
	sim->execute();

	return 0;
}