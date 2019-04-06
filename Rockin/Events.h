#pragma once

#include "CoreEntity.h"
#include <string>
#include <vector>

namespace Core
{
	class ScriptComponent;
	struct TilesetData;

	struct CollisionEvent : public sde::EventBase
	{
		ScriptComponent *receiver;
		CoreEntity *collider;
	};

	struct InputEvent : public sde::EventBase
	{
		float stickLeftX = 0.0f;
		float stickLeftY = 0.0f;
		float stickRightX = 0.0f;
		float stickRightY = 0.0f;
		int stickLeftPush = 0;
		int stickRightPush = 0;
		float padX = 0.0f;
		float padY = 0.0f;
		float trigger = 0.0f;
		int a = 0;
		int b = 0;
		int x = 0;
		int y = 0;
		int shoulderLeft = 0;
		int shoulderRight = 0;
		int select = 0;
		int start = 0;
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

	struct ChangeSceneEvent : public sde::EventBase
	{
		std::string sceneName;
		bool subscene = false;
	};

	struct NewSceneEvent : public sde::EventBase
	{
		std::vector<std::string> layer;
		std::vector<bool> isStatic;
		sf::Vector2i sceneSize;
		sf::Vector2i cellSize;
		TilesetData *tilesetData;
		sf::Vector2i tilemapSize;
		std::vector<int> tilemap;
		std::string tilemapLayer;
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
