#pragma once

#include "CoreEntity.h"
#include <string>

namespace Core
{
	class ScriptComponent;

	struct CollisionEvent : public sde::EventBase
	{
		ScriptComponent *receiver;
		CoreEntity *collider;
	};

	struct InputEvent : public sde::EventBase
	{
		float stickX{ 0.0f };
		float stickY{ 0.0f };
		bool fire{ false };
	};


	struct SoundEvent : public sde::EventBase
	{
		SoundEvent()
		{}
		SoundEvent(const std::string &tag, float volume, bool loop, int priority) :
			tag{ tag }, volume{ volume }, loop{ loop }, priority{ priority }
		{}
		std::string tag;
		float volume;
		bool loop;
		int priority;
	};
}
