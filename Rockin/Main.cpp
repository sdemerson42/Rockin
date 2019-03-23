#include <memory>
#include "Sim.h"
#include <time.h>

int main()
{
	srand(std::time(NULL));

	std::unique_ptr<Core::Sim> sim = std::make_unique<Core::Sim>(800, 600, "Rockin'");
	sim->execute();

	return 0;
}