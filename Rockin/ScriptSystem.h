#pragma once

#include "sde.h"
#include "angelscript.h"
#include "Events.h"
#include <stack>

namespace Core
{

	/*
	ScriptSystem - Processes ScriptComponents' suspension cycles and calls each component's
	asIScriptContext->Execute() method when appropriate. Calls scripts' collision methods
	upon receipt of CollisionEvents.

	NOTE: The scripting API is located in ScriptComponent rather than ScriptSystem in order
	to simplify scripting syntax.
	*/

	class ScriptSystem : public sde::ISystem
	{
	public:
		ScriptSystem();
		void execute() override;
	private:
		void onCollision(const CollisionEvent *c);
		void onSetMainScriptFunction(const SetMainScriptFunctionEvent *event);

		std::stack<SetMainScriptFunctionEvent> m_setMainEventStack;
	};
}
