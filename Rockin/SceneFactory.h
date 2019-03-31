#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "Events.h"
#include "sde.h"
#include "TilesetData.h"
#include <map>

namespace Core
{
	class EntityFactory;
	class CoreEntity;

	class SceneFactory : public sde::EventHandler
	{
	public:
		SceneFactory(EntityFactory *eFactory, std::vector<std::unique_ptr<CoreEntity>> *eVec);
		void buildScene();
		void buildScene(const std::string &name, bool isSubscene = false);
		void setSubscene(const std::string &name);
	private:
		struct EntityData
		{
			std::string name;
			int total;
			bool active;
			std::string layer;
			float x;
			float y;
		};
		struct LayerData
		{
			std::string name;
			bool isStatic;
		};
		struct SceneData
		{
			SceneData() :
				tilesetData{ nullptr }
			{}
			std::string name;
			std::vector<LayerData> layer;
			std::vector<EntityData> entity;
			sf::Vector2i sceneSize;
			sf::Vector2i cellSize;
			TilesetData *tilesetData;
			sf::Vector2i tilemapSize;
			std::vector<int> tilemap;
			std::string tilemapLayer;
			std::vector<std::string> subscene;
		};
		std::vector<SceneData> m_sceneData;
		std::vector<TilesetData> m_tilesetData;
		std::map<std::string, NewSceneEvent> m_newSceneEventMap;

		EntityFactory *m_eFactory;
		std::vector<std::unique_ptr<CoreEntity>> *m_eVec;

		void readSceneData();
		bool readScene(std::istream &ist, SceneData &sd);
		void readTilesetData();
		std::string nextToken(std::istream &ist);
	};
}
