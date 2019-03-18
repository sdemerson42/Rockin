#pragma once

#include "sde.h"

namespace Core
{
	class InputSystem : public sde::ISystem
	{
	public:
		void execute() override;
	};
}
