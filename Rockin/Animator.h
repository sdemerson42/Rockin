#pragma once

#include "sde.h"

namespace Core
{
	class Animator : public sde::ISystem
	{
	public:
		void execute() override;
	};
}