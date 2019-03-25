#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "Events.h"
#include "sde.h"

namespace Core
{
	class EntityFactory;
	class CoreEntity;

	class SceneFactory : public sde::EventHandler
	{
	public:
		SceneFactory(EntityFactory *eFactory, std::vector<std::unique_ptr<CoreEntity>> *eVec);
		void buildScene();
		void buildScene(const std::string &name);
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
			std::string name;
			std::vector<LayerData> layer;
			std::vector<EntityData> entity;
			sf::Vector2i sceneSize;
			sf::Vector2i cellSize;
		};
		std::vector<SceneData> m_sceneData;
		
		EntityFactory *m_eFactory;
		std::vector<std::unique_ptr<CoreEntity>> *m_eVec;

		void readSceneData();
		bool readScene(std::istream &ist, SceneData &sd);
		std::string nextToken(std::istream &ist);
	};
}
