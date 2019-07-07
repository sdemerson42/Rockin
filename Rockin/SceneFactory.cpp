#include "SceneFactory.h"
#include "CoreEntity.h"
#include <iostream>
#include "Events.h"
#include <algorithm>
#include "EntityFactory.h"
#include "Logger.h"
#include "DataIO.h"
#include "CoreException.h"

#include "Components.h"

namespace Core
{
	SceneFactory::SceneFactory(EntityFactory *eFactory, std::vector<std::unique_ptr<CoreEntity>> *eVec) :
		m_eFactory{ eFactory }, m_eVec{ eVec }
	{
		readTilesetData();
		readSceneData();
	}

	void SceneFactory::buildScene()
	{
		if (m_sceneData.size() > 0) buildScene(m_sceneData[0].name);
		else
		{
			CoreException e{ "WARNING: Default scene data not found.", 4 };
			throw(e);
		}
	}

	void SceneFactory::buildScene(const std::string &sceneName, bool isSubscene)
	{
		NewSceneEvent nse;

		if (!isSubscene)
		{
			auto iter = std::begin(*m_eVec);
			while (iter < std::end(*m_eVec))
			{
				if ((*iter)->persistent())
				{
					moveEntityIntoScene(iter->get(), sceneName);
					++iter;
				}
				else
				{
					m_eVec->erase(iter, std::end(*m_eVec));
					break;
				}
			}
		}

		// Find SceneData

		auto sd = std::find_if(std::begin(m_sceneData), std::end(m_sceneData), [&](const SceneData &sd)
		{
			return sd.name == sceneName;
		});
		if (sd == std::end(m_sceneData))
		{
			Logger::log("WARNING: Scene not found. Construction aborted.");
			return;
		}

		// Get layer data

		for (const LayerData &ld : sd->layer)
		{
			nse.layer.push_back(ld.name);
			nse.isStatic.push_back(ld.isStatic);
		}

		// Create entities (persistent first)

		for (EntityData &ed : sd->entity)
		{
			if (ed.persistent && !ed.persistentCreated)
			{
				for (int i = 0; i < ed.total; ++i)
				{
					m_eFactory->createEntity(ed.name, ed.active, ed.persistent, ed.layer, ed.x, ed.y, sceneName);
					if (ed.data.size() > 0) m_eFactory->addInitData(ed.data);
					ed.persistentCreated = true;
				}
			}
		}

		for (const EntityData &ed : sd->entity)
		{
			if (!ed.persistent)
			{
				for (int i = 0; i < ed.total; ++i)
				{
					m_eFactory->createEntity(ed.name, ed.active, ed.persistent, ed.layer, ed.x, ed.y, sceneName);
					if (ed.data.size() > 0) m_eFactory->addInitData(ed.data);
				}
			}
		}

		// Copy tilemap data if present

		if (sd->tilesetData)
		{
			nse.tilesetData = sd->tilesetData;
			nse.tilemapSize = sd->tilemapSize;
			nse.tilemap = sd->tilemap;
			nse.tilemapLayer = sd->tilemapLayer;

			// Create tilemap physics Entity

			m_eVec->push_back(std::make_unique<CoreEntity>());
			auto e = m_eVec->back().get();

			e->setActive(true);
			e->setPosition(0.0f, 0.0f);

			// Simplify geometry

			std::vector<sf::IntRect> AABB;
			int mapX = nse.tilemapSize.x;
			int mapY = nse.tilemapSize.y;
			int tileX = nse.tilesetData->tileSize.x;
			int tileY = nse.tilesetData->tileSize.y;
			std::vector<std::vector<int>> physMap;
			physMap.resize(nse.tilemapSize.x);
			for (auto &v : physMap)
			{
				v.resize(nse.tilemapSize.y);
			}

			for (int i = 0; i < nse.tilemap.size(); ++i)
			{
				int tVal = nse.tilemap[i];
				if (std::find(std::begin(sd->tilesetData->staticTile),
					std::end(sd->tilesetData->staticTile), tVal) != std::end(sd->tilesetData->staticTile))
				{
					int x = (i % mapX);
					int y = (i / mapX);
					physMap[x][y] = 1;
				}
			}

			for (int j = 0; j < nse.tilemapSize.y; ++j)
			{
				for (int i = 0; i < nse.tilemapSize.x; ++i)
				{
					if (physMap[i][j] == 1)
					{
						int m = i + 1;
						while (m < nse.tilemapSize.x && physMap[m][j] == 1) ++m;
						int n = j + 1;
						while (true)
						{
							bool rowFlag = true;
							for (int k = i; k < m; ++k)
							{
								if (n >= nse.tilemapSize.y || physMap[k][n] != 1)
								{
									rowFlag = false;
									break;
								}
							}
							if (!rowFlag) break;
							++n;
						}
						// Remove this block from physMap and translate to Physics data

						for (int ii = i; ii < m; ++ii)
						{
							for (int jj = j; jj < n; ++jj)
							{
								physMap[ii][jj] = -1;
							}
						}
						AABB.push_back(sf::IntRect{ i, j, m - i, n - j });
					}
				}
			}

			// Create PhysicsComponents from data

			for (const auto &aabb : AABB)
			{
				e->addComponent<PhysicsComponent>(e, (float)aabb.left * (float)tileX, (float)aabb.top * (float)tileY,
					(float)aabb.width * (float)tileX, (float)aabb.height * (float)tileY, 
					0.0f, 0.0f, 0.0f, 0.0f, true, true, false);
			}

			auto pcv = e->getComponents<PhysicsComponent>();
			for (auto pc : pcv)
			{
				pc->alsMoveRef(sceneName);
			}
		}
		else nse.tilesetData = nullptr;

		nse.sceneSize = sd->sceneSize;
		nse.cellSize = sd->cellSize;

		// Broadcast NewSceneEvent

		m_newSceneEventMap[sceneName] = nse;

		// Build subscenes

		if (!isSubscene)
			setSubscene(sceneName);

		for (const auto &subsceneName : sd->subscene)
		{
			buildScene(subsceneName, true);
		}

		Logger::log(sceneName + " built successfully.");
	}

	void SceneFactory::setSubscene(const std::string &name)
	{
		// Ensure subscene exists. Otherwise throw exception.

		auto p = std::find_if(std::begin(m_sceneData), std::end(m_sceneData),
			[&](const SceneData &sd)
		{
			return sd.name == name;
		});

		if (p == std::end(m_sceneData))
		{
			CoreException e{ "Tried to contruct nonexistent subscene.", 5 };
			throw(e);
		}

		AutoListScene<AnimationComponent>::alsSetScene(name);
		AutoListScene<PhysicsComponent>::alsSetScene(name);
		AutoListScene<RenderComponent>::alsSetScene(name);
		AutoListScene<ScriptComponent>::alsSetScene(name);
		AutoListScene<TextComponent>::alsSetScene(name);

		broadcast(&m_newSceneEventMap[name]);
	}

	void SceneFactory::moveEntityIntoScene(CoreEntity *e, const std::string &name)
	{
		auto v = e->getComponents<AnimationComponent>();
		for (auto cp : v)
		{
			cp->alsMoveRef(name);
		}
		
		auto v2 = e->getComponents<PhysicsComponent>();
		for (auto cp : v2)
		{
			cp->alsMoveRef(name);
		}
		auto v3 = e->getComponents<RenderComponent>();
		for (auto cp : v3)
		{
			cp->alsMoveRef(name);
		}
		auto v4 = e->getComponents<ScriptComponent>();
		for (auto cp : v4)
		{
			cp->alsMoveRef(name);
		}
		auto v5 = e->getComponents<TextComponent>();
		for (auto cp : v5)
		{
			cp->alsMoveRef(name);
		}
	}

	void SceneFactory::readSceneData()
	{
		std::ifstream ifs{ "Data/Scenes.dat" };
		if (!ifs)
		{
			CoreException e{ "Scene data not found.", 6 };
			throw(e);
		}

		while (true)
		{
			SceneData sd;
			if (!readScene(ifs, sd))
				break;
			m_sceneData.push_back(sd);
		}
	}

	bool SceneFactory::readScene(std::istream &ist, SceneData &sd)
	{		
		auto t = Tokenizer::next(ist);
		if (t == " ") return false;

		sd.name = t;

		t = Tokenizer::next(ist);
		if (t != "{")
		{
			Logger::log("WARNING: Bad scene formatting.");
			return false;
		}

		while (true)
		{
			t = Tokenizer::next(ist);
			if (t == "Layer")
			{
				// Read Layer data

				LayerData ld;

				t = Tokenizer::next(ist);
				if (t != "{")
				{
					Logger::log("WARNING: Bad layer formatting.");
					return false;
				}

				t = Tokenizer::next(ist);
				ld.name = t;
				t = Tokenizer::next(ist);
				if (t != ",")
				{
					Logger::log("WARNING: Bad layer formatting.");
					return false;
				}
				t = Tokenizer::next(ist);
				ld.isStatic = (t == "static" ? true : false);
				t = Tokenizer::next(ist);
				if (t != "}")
				{
					Logger::log("WARNING: Bad layer formatting.");
					return false;
				}
				sd.layer.push_back(ld);
			}
			else if (t == "Entity" || t == "PersistentEntity")
			{
				// Read entity data

				EntityData ed;

				if (t == "PersistentEntity")
				{
					ed.persistent = true;
					ed.persistentCreated = false;
				}
				else
					ed.persistent = false;

				t = Tokenizer::next(ist);
				if (t != "{")
				{
					Logger::log("WARNING: Bad entity formatting.");
					return false;
				}
				t = Tokenizer::next(ist);
				ed.name = t;
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				ed.total = std::stoi(t);
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				ed.active = (t == "true" ? true : false);
				t = Tokenizer::next(ist);
				if (t == "}")
				{
					ed.layer = "default";
					ed.x = 0.0f;
					ed.y = 0.0f;
					sd.entity.push_back(ed);
					continue;
				}
				t = Tokenizer::next(ist);
				ed.layer = t;
				t = Tokenizer::next(ist);
				if (t == "}")
				{
					ed.x = 0.0f;
					ed.y = 0.0f;
					sd.entity.push_back(ed);
					continue;
				}
				t = Tokenizer::next(ist);
				ed.x = std::stof(t);
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				ed.y = std::stof(t);
				sd.entity.push_back(ed);
				t = Tokenizer::next(ist);
			}
			else if (t == "Physics")
			{
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				sd.sceneSize.x = std::stoi(t);
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				sd.sceneSize.y = std::stoi(t);
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				sd.cellSize.x = std::stoi(t);
				t = Tokenizer::next(ist);
				t = Tokenizer::next(ist);
				sd.cellSize.y = std::stoi(t);
				t = Tokenizer::next(ist);
			}
			else if (t == "Tilemap")
			{
				Tokenizer::next(ist);
				std::string tmName = Tokenizer::next(ist);

				auto iter = std::find_if(std::begin(m_tilesetData), std::end(m_tilesetData), [&](const TilesetData &td)
				{
					return td.name == tmName;
				});

				if (iter == std::end(m_tilesetData))
				{
					Logger::log("WARNING: Tileset not found.");
					return false;
				}

				sd.tilesetData = &*iter;
				t = Tokenizer::next(ist);
				sd.tilemapLayer = Tokenizer::next(ist);
				Tokenizer::next(ist);
				sd.tilemapSize.x = std::stoi(Tokenizer::next(ist));
				Tokenizer::next(ist);
				sd.tilemapSize.y = std::stoi(Tokenizer::next(ist));

				int sz = sd.tilemapSize.x * sd.tilemapSize.y;
				for (int i = 0; i < sz; ++i)
				{
					Tokenizer::next(ist);
					sd.tilemap.push_back(std::stoi(Tokenizer::next(ist)));
				}

				if (Tokenizer::next(ist) != "}")
				{
					Logger::log("WARNING: Bad tilemap formatting.");
					return false;
				}
			}
			else if (t == "Subscene")
			{
				Tokenizer::next(ist);
				sd.subscene.push_back(Tokenizer::next(ist));
				Tokenizer::next(ist);
			}
			else if (t == "Data")
			{
				if (sd.entity.size() == 0) break;

				EntityData &ed = sd.entity[sd.entity.size() - 1];
				Tokenizer::next(ist);
				while (true)
				{
					ed.data.push_back(Tokenizer::next(ist));
					Tokenizer::next(ist);
					ed.data.push_back(Tokenizer::next(ist));
					if (Tokenizer::next(ist) == "}") break;
				}
			}
			else if (t != "}")
			{
				Logger::log("WARNING: Bad scene formatting.");
				return false;
			}
			else break;
		}
		return true;
	}

	void SceneFactory::readTilesetData()
	{
		std::ifstream ifs{ "Data/Tilesets.dat" };
		if (!ifs)
		{
			Logger::log("WARNING: Tileset data not found.");
			return;
		}

		while (true)
		{
			auto t = Tokenizer::next(ifs);

			if (t == " ") break;
			
			TilesetData td;
			td.name = t;
			Tokenizer::next(ifs);
			td.texture = Tokenizer::next(ifs);
			Tokenizer::next(ifs);
			int x = std::stoi(Tokenizer::next(ifs));
			Tokenizer::next(ifs);
			int y = std::stoi(Tokenizer::next(ifs));
			td.textureSize = { x, y };

			Tokenizer::next(ifs);
			int tx = std::stoi(Tokenizer::next(ifs));
			Tokenizer::next(ifs);
			int ty = std::stoi(Tokenizer::next(ifs));
			td.tileSize = { tx, ty };
			t = Tokenizer::next(ifs);
			if (t != "}")
			{
				do
				{
					td.staticTile.push_back(std::stoi(Tokenizer::next(ifs)));
					t = Tokenizer::next(ifs);
				} while (t != "}");
			}
			m_tilesetData.push_back(td);
		}
	}
}