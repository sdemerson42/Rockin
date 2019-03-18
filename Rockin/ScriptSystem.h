#pragma once

#include "sde.h"
#include "angelscript.h"
#include "Events.h"

namespace Core
{
	class ScriptSystem : public sde::ISystem
	{
	public:
		ScriptSystem();
		void execute() override;
	private:
		void onCollision(const CollisionEvent *c);
	};
}
