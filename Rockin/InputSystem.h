#pragma once

#include "sde.h"
#include "Events.h"

namespace Core
{
	/*
	InputSystem - Records input from keyboard / controller and stores static data within the ScriptComponent
	class for access by scripting API. InputEvents can also be broadcast to EventHandlers as necessary.

	NOTE: Currently keyboard and controller bindings are hardcoded in InputSystem.cpp and cannot be
	altered during runtime. Rebindable input and mouse input will be added in the future.
	*/

	class InputSystem : public sde::ISystem
	{
	public:
		void execute() override;
	private:
		InputEvent m_ie;
		const float deadZone = 0.2f;
	};
}
