#pragma once

#include "CoreEntity.h"
#include <string>
#include <vector>

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

	struct NewSceneEvent : public sde::EventBase
	{
		std::vector<std::string> layer;
		std::vector<bool> isStatic;
		sf::Vector2i sceneSize;
		sf::Vector2i cellSize;
	};

	struct SetViewCenterEvent : public sde::EventBase
	{
		SetViewCenterEvent()
		{}
		SetViewCenterEvent(float _x, float _y) :
			x{ _x }, y{ _y }
		{}
		float x;
		float y;
	};
}
