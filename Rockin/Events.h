#pragma once

#include "CoreEntity.h"
#include <string>
#include <vector>

namespace Core
{
	/*
	All EventBase structs that are to be passed among sde::EventHandlers should
	be defined here.
	*/

	class ScriptComponent;
	class PhysicsComponent;
	struct TilesetData;

	struct CollisionEvent : public sde::EventBase
	{
		ScriptComponent *receiver;
		CoreEntity *collider;
		PhysicsComponent *colliderPhysics;
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
		SoundEvent(const std::string &tag, float volume, bool loop, int priority, bool stop = false) :
			tag{ tag }, volume{ volume }, loop{ loop }, priority{ priority }, stop{ stop }
		{}
		std::string tag;
		float volume;
		bool loop;
		int priority;
		bool stop;
	};

	struct MusicEvent : public sde::EventBase
	{
		MusicEvent() :
			action{ 0 }
		{}
		std::string fileName;
		float volume;
		bool loop;
		int action;
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

	struct NewSceneDataEvent : public sde::EventBase
	{
		std::string name;
		int physWidth;
		int physHeight;
		int physCellWidth;
		int physCellHeight;
	};

	struct AddSceneLayerEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string layerName;
		bool isStatic;
	};

	struct AddSceneTilemapEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string tilesetName;
		std::string layer;
		int width;
		int height;
		std::vector<int> tiles;
	};

	struct AddSceneEntityEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string entityName;
		int count;
		bool instantSpawn;
		std::string layer;
		float posX;
		float posY;
		bool persist;
		std::vector<std::string> data;
	};

	struct AddSubsceneEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string subsceneName;
	};

	struct AddSceneBaseEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string baseName;
	};
	
	struct SetMainScriptFunctionEvent : public sde::EventBase
	{
		ScriptComponent *scriptComponent;
		std::string funcName;
	};

	struct RemoveSceneEntityEvent : public sde::EventBase
	{
		std::string sceneName;
		std::string entityName;
	};

	struct TilemapEditEvent : public sde::EventBase
	{
	};

	struct TilesetDataQueryEvent : public sde::EventBase 
	{
		std::string tilesetName;
	};
}
