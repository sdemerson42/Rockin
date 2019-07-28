#pragma once

#include "sde.h"

namespace Core
{
	class Particles : public sde::ISystem
	{
	public:
		void execute() override;
	private:
		static const float pi;
		static const float radPerDeg;
	};
}
