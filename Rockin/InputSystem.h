#pragma once

#include "sde.h"
#include "Events.h"

namespace Core
{
	class InputSystem : public sde::ISystem
	{
	public:
		void execute() override;
	private:
		InputEvent m_ie;
		const float deadZone = 0.2f;
	};
}
