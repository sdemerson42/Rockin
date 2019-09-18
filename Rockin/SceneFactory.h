#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "Events.h"
#include "sde.h"
#include "TilesetData.h"
#include <map>
#include <set>

namespace Core
{
	/*
	SceneFactory - Stores Scene data and provides methods for construction of Scenes and Subscenes.
	*/
	class EntityFactory;
	class CoreEntity;

	class SceneFactory : public sde::EventHandler
	{
	public:
		SceneFactory(EntityFactory *eFactory, std::vector<std::unique_ptr<CoreEntity>> *eVec);
		std::string buildScene();
		std::string buildScene(const std::string &name, bool isSubscene = false);
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
			bool persistent;
			bool persistentCreated;
			std::vector<std::string> data;
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
		std::map<std::string, std::set<std::string>> m_inheritanceData;

		// References to existing objects in Sim
		EntityFactory *m_eFactory;
		std::vector<std::unique_ptr<CoreEntity>> *m_eVec;
		
		// Entity creation
		void createEntities(SceneData &sd, const std::string &sceneName);
		void processTilemapData(SceneData &sd, NewSceneEvent &nse, const std::string &sceneName);
		void processPhysicsGeometry(SceneData &sd, NewSceneEvent &nse, CoreEntity *e);
		void moveEntityIntoScene(CoreEntity *e, const std::string &scene);
		
		// Data input methods
		void readScenes();
		bool readSceneDataTokens(std::istream &ist, SceneData &sd);
		bool readLayerData(std::istream &ist, SceneData &sd);
		bool readEntityData(std::istream &ist, SceneData &sd, const std::string &persistType);
		bool readPhysicsData(std::istream &ist, SceneData &sd);
		bool readTilemapData(std::istream &ist, SceneData &sd);
		bool readEntityInitData(std::istream &ist, SceneData &sd);
		void readTilesetData();
		void combineInhenitanceData();

		// Helper methods
		auto findScene(const std::string &sceneName);

		// Event handler methods
		void onNewSceneData(const NewSceneDataEvent *event);
		void onAddSceneLayer(const AddSceneLayerEvent *event);
		void onAddSceneEntity(const AddSceneEntityEvent *event);
		void onAddSceneTilemap(const AddSceneTilemapEvent *event);
		void onAddSubscene(const AddSubsceneEvent *event);
		void onRemoveSceneEntity(const RemoveSceneEntityEvent *event);
		void onAddSceneBase(const AddSceneBaseEvent *event);
		void onTilesetDataQuery(const TilesetDataQueryEvent *event);
	};
}
