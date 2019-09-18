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
		registerFunc(this, &SceneFactory::onAddSceneEntity);
		registerFunc(this, &SceneFactory::onAddSceneLayer);
		registerFunc(this, &SceneFactory::onAddSceneTilemap);
		registerFunc(this, &SceneFactory::onAddSubscene);
		registerFunc(this, &SceneFactory::onNewSceneData);
		registerFunc(this, &SceneFactory::onRemoveSceneEntity);
		registerFunc(this, &SceneFactory::onAddSceneBase);
		registerFunc(this, &SceneFactory::onTilesetDataQuery);
	
		readTilesetData();
		readScenes();
	}

	std::string SceneFactory::buildScene()
	{
		std::string sceneName;

		if (m_sceneData.size() > 0) sceneName = buildScene(m_sceneData[0].name);
		else
		{
			CoreException e{ "WARNING: Default scene data not found.", CoreException::ErrCode::missingDefaultSceneData };
			throw(e);
		}
		return sceneName;
	}

	std::string SceneFactory::buildScene(const std::string &sceneName, bool isSubscene)
	{
		NewSceneEvent nse;

		// Perform cleanup before building

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
			return "";
		}

		// Get layer data

		for (const LayerData &ld : sd->layer)
		{
			nse.layer.push_back(ld.name);
			nse.isStatic.push_back(ld.isStatic);
		}

		// Create entities (persistent first)

		createEntities(*sd, sceneName);

		// Copy tilemap data if present

		if (sd->tilesetData) processTilemapData(*sd, nse, sceneName);
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
		return sceneName;
	}

	void SceneFactory::createEntities(SceneData &sd, const std::string &sceneName)
	{
		for (EntityData &ed : sd.entity)
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

		for (const EntityData &ed : sd.entity)
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
	}

	void SceneFactory::processTilemapData(SceneData &sd, NewSceneEvent &nse, const std::string &sceneName)
	{
		nse.tilesetData = sd.tilesetData;
		nse.tilemapSize = sd.tilemapSize;
		nse.tilemap = sd.tilemap;
		nse.tilemapLayer = sd.tilemapLayer;

		// Create tilemap physics Entity

		m_eVec->push_back(std::make_unique<CoreEntity>());
		auto e = m_eVec->back().get();

		e->setActive(true);
		e->setPosition(0.0f, 0.0f);

		// Process solid tiles and construct physics components

		processPhysicsGeometry(sd, nse, e);

		// Move references to newly created PhysicsComponents
		// to the current scene AutoList

		auto pcv = e->getComponents<PhysicsComponent>();
		for (auto pc : pcv)
		{
			pc->alsMoveRef(sceneName);
		}
	}

	void SceneFactory::processPhysicsGeometry(SceneData &sd, NewSceneEvent &nse, CoreEntity *e)
	{
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

		// Create a physical map that represents placement of tiles with solid geometry

		for (int i = 0; i < nse.tilemap.size(); ++i)
		{
			int tVal = nse.tilemap[i];
			if (std::find(std::begin(sd.tilesetData->staticTile),
				std::end(sd.tilesetData->staticTile), tVal) != std::end(sd.tilesetData->staticTile))
			{
				int x = (i % mapX);
				int y = (i / mapX);
				physMap[x][y] = 1;
			}
		}

		// Find rectangular groups of solid tiles and replace them with a single
		// AABB data entry

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
			CoreException e{ "Tried to contruct nonexistent subscene.", CoreException::ErrCode::missingSubsceneData };
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

	void SceneFactory::readScenes()
	{
		std::ifstream ifs{ "Data/Scenes.dat" };
		if (!ifs)
		{
			CoreException e{ "Scene data not found.", CoreException::ErrCode::missingSceneData };
			throw(e);
		}

		while (true)
		{
			SceneData sd;
			if (!readSceneDataTokens(ifs, sd))
				break;
			m_sceneData.push_back(sd);
		}
		combineInhenitanceData();
	}

	bool SceneFactory::readSceneDataTokens(std::istream &ist, SceneData &sd)
	{	
		if (!ist) return false;
		auto token = Tokenizer::next(ist);
		if (token == " ") return false;

		sd.name = token;

		token = Tokenizer::next(ist);
		if (!checkDataFormatting(token, "{", "WARNING: Bad scene formatting.")) return false;
		
		std::string badSubsceneMsg{ "WARNING: Bad subscene formatting in scene " + sd.name + "." };

		while (true)
		{
			// Read tokens and add to SceneData based on information found in file

			token = Tokenizer::next(ist);
			if ("Layer" == token)
			{
				if (!readLayerData(ist, sd)) return false;
			}
			else if ("Entity" == token  || "PersistentEntity" == token)
			{
				if (!readEntityData(ist, sd, token)) return false;
			}
			else if ("Physics" == token)
			{
				if (!readPhysicsData(ist, sd)) return false;
			}
			else if ("Tilemap" == token)
			{
				if (!readTilemapData(ist, sd)) return false;
			}
			else if ("Subscene" == token)
			{
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, "{", badSubsceneMsg)) return false;
				sd.subscene.push_back(Tokenizer::next(ist));
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, "}", badSubsceneMsg)) return false;
			}
			else if ("Data" == token)
			{
				if (!readEntityInitData(ist, sd)) return false;
			}
			else if ("Base" == token)
			{
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, "{", badSubsceneMsg)) return false;
				m_inheritanceData[sd.name].insert(Tokenizer::next(ist));
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, "}", badSubsceneMsg)) return false;
			}
			else if ("}" != token)
			{
				Logger::log("WARNING: Bad scene formatting.");
				return false;
			}
			else break;
		}
		return true;
	}

	bool SceneFactory::readLayerData(std::istream &ist, SceneData &sd)
	{
		try
		{
			std::string badMsg{ "WARNING: Bad layer formatting in scene " + sd.name + "." };
			LayerData ld;

			auto token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "{", badMsg)) return false;

			token = Tokenizer::next(ist);
			ld.name = token;
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ld.isStatic = (token == "static" ? true : false);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "}", badMsg)) return false;
			sd.layer.push_back(ld);

			return true;
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading layer data, scene " + sd.name + ".");
			throw(e);
		}
	}

	bool SceneFactory::readEntityData(std::istream &ist, SceneData &sd, const std::string &persistType)
	{
		try
		{
			std::string badMsg{ "WARNING: Bad Entity formatting in scene " + sd.name + "." };
			EntityData ed;

			if ("PersistentEntity" == persistType)
			{
				ed.persistent = true;
				ed.persistentCreated = false;
			}
			else
				ed.persistent = false;

			auto token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "{", badMsg)) return false;

			token = Tokenizer::next(ist);
			ed.name = token;
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ed.total = std::stoi(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ed.active = (token == "true" ? true : false);

			// Set optional data to defaults

			ed.layer = "default";
			ed.x = 0.0f;
			ed.y = 0.0f;

			token = Tokenizer::next(ist);
			if ("}" == token)
			{
				sd.entity.push_back(ed);
				return true;
			}
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ed.layer = token;
			token = Tokenizer::next(ist);
			if ("}" == token)
			{
				sd.entity.push_back(ed);
				return true;
			}
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ed.x = std::stof(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			ed.y = std::stof(token);
			sd.entity.push_back(ed);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "}", badMsg)) return false;
			return true;
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading entity data, scene " + sd.name + ".");
			throw(e);
		}
	}

	bool SceneFactory::readPhysicsData(std::istream &ist, SceneData &sd)
	{
		try
		{
			std::string badMsg{ "WARNING: Bad physics formatting in scene " + sd.name + "." };

			auto token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "{", badMsg)) return false;
			token = Tokenizer::next(ist);
			sd.sceneSize.x = std::stoi(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			sd.sceneSize.y = std::stoi(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			sd.cellSize.x = std::stoi(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			token = Tokenizer::next(ist);
			sd.cellSize.y = std::stoi(token);
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "}", badMsg)) return false;
			return true;
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading physics data, scene " + sd.name + ".");
			throw(e);
		}
	}

	bool SceneFactory::readEntityInitData(std::istream &ist, SceneData &sd)
	{
		try
		{
			std::string badMsg{ "WARNING: Bad entity initialization data formatting in scene " + sd.name + "." };

			// Return immediately if there is no Entity to reference
			if (sd.entity.size() == 0)
			{
				Logger::log(badMsg);
				return false;
			}

			EntityData &ed = sd.entity[sd.entity.size() - 1];
			auto token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "{", badMsg)) return false;
			while (true)
			{
				// Error conditions

				if (!ist)
				{
					Logger::log(badMsg);
					return false;
				}
				
				// Read data
				ed.data.push_back(Tokenizer::next(ist));
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, ",", badMsg)) return false;
				ed.data.push_back(Tokenizer::next(ist));
				token = Tokenizer::next(ist);
				if ("}" == token) break;
				if (!checkDataFormatting(token, ",", badMsg)) return false;
			}
			return true;
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading Entity initialization data, scene " + sd.name + ".");
			throw(e);
		}
	}

	bool SceneFactory::readTilemapData(std::istream &ist, SceneData &sd)
	{
		try
		{
			std::string badMsg{ "WARNING: Bad tilemap formatting in scene " + sd.name + "." };

			auto token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "{", badMsg)) return false;
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
			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			sd.tilemapLayer = Tokenizer::next(ist);
			Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			sd.tilemapSize.x = std::stoi(Tokenizer::next(ist));
			Tokenizer::next(ist);
			if (!checkDataFormatting(token, ",", badMsg)) return false;
			sd.tilemapSize.y = std::stoi(Tokenizer::next(ist));

			int sz = sd.tilemapSize.x * sd.tilemapSize.y;
			for (int i = 0; i < sz; ++i)
			{
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, ",", badMsg)) return false;
				sd.tilemap.push_back(std::stoi(Tokenizer::next(ist)));
			}

			token = Tokenizer::next(ist);
			if (!checkDataFormatting(token, "}", badMsg)) return false;
			return true;
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading tilemap data, scene " + sd.name + ".");
			throw(e);
		}
	}

	void SceneFactory::readTilesetData()
	{
		std::ifstream ifs{ "Data/Tilesets.dat" };
		if (!ifs)
		{
			Logger::log("WARNING: Tileset data not found.");
			return;
		}

		std::string currentTilesetName{ "UNKNOWN" };

		try
		{
			while (true)
			{
				std::string badMsg{ "WARNING: Bad tileset formatting while reading unknown tileset." };

				auto token = Tokenizer::next(ifs);

				if (" " == token) break;

				TilesetData td;
				td.name = currentTilesetName = token;
				badMsg = "WARNING: Bad tileset formatting while reading tileset " + currentTilesetName;
				token = Tokenizer::next(ifs);
				if (!checkDataFormatting(token, "{", badMsg)) return;
				td.texture = Tokenizer::next(ifs);
				token = Tokenizer::next(ifs);
				if (!checkDataFormatting(token, ",", badMsg)) return;
				int x = std::stoi(Tokenizer::next(ifs));
				token = Tokenizer::next(ifs);
				if (!checkDataFormatting(token, ",", badMsg)) return;
				int y = std::stoi(Tokenizer::next(ifs));
				td.textureSize = { x, y };

				token = Tokenizer::next(ifs);
				if (!checkDataFormatting(token, ",", badMsg)) return;
				int tx = std::stoi(Tokenizer::next(ifs));
				token = Tokenizer::next(ifs);
				if (!checkDataFormatting(token, ",", badMsg)) return;
				int ty = std::stoi(Tokenizer::next(ifs));
				td.tileSize = { tx, ty };
				token = Tokenizer::next(ifs);
				if ("}" != token)
				{
					while(true)
					{
						if (!ifs)
						{
							Logger::log(badMsg);
							return;
						}
						td.staticTile.push_back(std::stoi(Tokenizer::next(ifs)));
						token = Tokenizer::next(ifs);
						if ("}" == token) break;
						if (!checkDataFormatting(token, ",", badMsg)) return;
					}
				}
				m_tilesetData.push_back(td);
			}
		}
		catch (const std::exception &e)
		{
			Logger::log("Exception thrown while reading tileset data in tileset " + currentTilesetName + ".");
			throw(e);
		}
	}

	void SceneFactory::combineInhenitanceData()
	{
		for (const auto &pr : m_inheritanceData)
		{
			auto derivedName = pr.first;
			std::string errMsg{ "Exception thrown while combining scene inheritance data for scene " + derivedName + "." };
			auto derivedIter = std::find_if(std::begin(m_sceneData), std::end(m_sceneData), [&](const SceneData &sd)
			{
				return sd.name == derivedName;
			});
			if (derivedIter == std::end(m_sceneData))
			{
				CoreException e{ errMsg.c_str(), CoreException::ErrCode::badSceneInheritanceData };
				throw(e);
			}

			for (const auto &baseName : pr.second)
			{
				auto baseIter = std::find_if(std::begin(m_sceneData), std::end(m_sceneData), [&](const SceneData &sd)
				{
					return sd.name == baseName;
				});
				if (baseIter == std::end(m_sceneData))
				{
					CoreException e{ errMsg.c_str(), CoreException::ErrCode::badSceneInheritanceData };
					throw(e);
				}
				// Add base data to derived data. Currently this feature only supports inheriting
				// Entity data and subscene data.
				for (const auto &ed : baseIter->entity)
				{
					derivedIter->entity.push_back(ed);
				}
				for (const auto &sub : baseIter->subscene)
				{
					derivedIter->subscene.push_back(sub);
				}
			}
		}
		m_inheritanceData.clear();
	}

	void SceneFactory::onNewSceneData(const NewSceneDataEvent *event)
	{
		SceneData sd;
		sd.tilesetData = nullptr;
		sd.name = event->name;
		sd.sceneSize.x = event->physWidth;
		sd.sceneSize.y = event->physHeight;
		sd.cellSize.x = event->physCellWidth;
		sd.cellSize.y = event->physCellHeight;
		m_sceneData.push_back(sd);
	}

	auto SceneFactory::findScene(const std::string &sceneName)
	{
		auto it = std::find_if(std::begin(m_sceneData), std::end(m_sceneData), [&](const SceneData &sd)
		{
			return sd.name == sceneName;
		});

		if (it == std::end(m_sceneData))
		{
			CoreException e{ "Scene data not found.", CoreException::ErrCode::missingSceneData };
			throw(e);
		}

		return it;
	}

	void SceneFactory::onAddSceneLayer(const AddSceneLayerEvent *event)
	{
		auto it = findScene(event->sceneName);

		LayerData ld;
		ld.name = event->layerName;
		ld.isStatic = event->isStatic;

		it->layer.push_back(ld);
	}

	void SceneFactory::onAddSceneEntity(const AddSceneEntityEvent *event)
	{
		auto it = findScene(event->sceneName);

		EntityData ed;
		ed.active = event->instantSpawn;
		ed.layer = event->layer;
		ed.name = event->entityName;
		ed.persistent = event->persist;
		ed.persistentCreated = false;
		ed.total = event->count;
		ed.x = event->posX;
		ed.y = event->posY;
		ed.data = event->data;

		it->entity.push_back(ed);
	}

	void SceneFactory::onAddSceneTilemap(const AddSceneTilemapEvent *event)
	{
		auto it = findScene(event->sceneName);
		auto tsp = std::find_if(std::begin(m_tilesetData), std::end(m_tilesetData), [&](const TilesetData &td)
		{
			return td.name == event->tilesetName;
		});
		if (tsp == std::end(m_tilesetData))
		{
			CoreException e{ "Tilset data not found.", CoreException::ErrCode::missingTilesetData };
		}

		it->tilemapLayer = event->layer;
		it->tilemapSize.x = event->width;
		it->tilemapSize.y = event->height;
		it->tilemap = event->tiles;
		it->tilesetData = &*tsp;
	}

	void SceneFactory::onAddSubscene(const AddSubsceneEvent *event)
	{
		auto it = findScene(event->sceneName);
		it->subscene.push_back(event->subsceneName);
	}

	void SceneFactory::onRemoveSceneEntity(const RemoveSceneEntityEvent *event)
	{
		auto it = findScene(event->sceneName);

		for (int i = 0; i < it->entity.size(); ++i)
		{
			auto &ed = it->entity[i];
			if (ed.name == event->entityName)
			{
				it->entity.erase(std::begin(it->entity) + i--);
			}
		}
	}

	void SceneFactory::onAddSceneBase(const AddSceneBaseEvent *event)
	{
		m_inheritanceData[event->sceneName].insert(event->baseName);
		combineInhenitanceData();
	}

	void SceneFactory::onTilesetDataQuery(const TilesetDataQueryEvent *event)
	{
		TilesetData *response = nullptr;
		auto p = std::find_if(std::begin(m_tilesetData), std::end(m_tilesetData), [&](TilesetData &tsd)
		{
			return tsd.name == event->tilesetName;
		});

		if (p != std::end(m_tilesetData)) response = &*p;
		ScriptComponent::setTilesetData(response);
	}
}