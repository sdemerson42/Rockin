#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Core
{
	class EntityFactory;
	
	class SceneFactory
	{
	public:
		SceneFactory(EntityFactory *eFactory);
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
		};
		std::vector<SceneData> m_sceneData;
		EntityFactory *m_eFactory;

		void readSceneData();
		bool readScene(std::istream &ist, SceneData &sd);
		std::string nextToken(std::istream &ist);
	};
}
