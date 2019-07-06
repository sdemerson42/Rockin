#pragma once

#include "sde.h"

namespace Core
{
	/*
	Animator - System that ensures that entities with AnimationComponents update their animation state
	at the appropriate times. State is owned by the components.
	*/

	class Animator : public sde::ISystem
	{
	public:
		void execute() override;
	};
}