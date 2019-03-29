#include "SceneFactory.h"
#include "CoreEntity.h"
#include <iostream>
#include "Events.h"
#include <algorithm>
#include "EntityFactory.h"
#include "PhysicsComponent.h"

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
		else std::cerr << "WARNING: Default scene data not found.\n";
	}

	void SceneFactory::buildScene(const std::string &name)
	{
		NewSceneEvent nse;
		
		// TO-DO: persistence

		m_eVec->clear();

		// Find SceneData

		auto sd = std::find_if(std::begin(m_sceneData), std::end(m_sceneData), [&](const SceneData &sd)
		{
			return sd.name == name;
		});
		if (sd == std::end(m_sceneData))
		{
			std::cerr << "WARNING: Scene not found. Construction aborted.\n";
			return;
		}

		// Get layer data

		for (const LayerData &ld : sd->layer)
		{
			nse.layer.push_back(ld.name);
			nse.isStatic.push_back(ld.isStatic);
		}

		// Create entities

		for (const EntityData &ed : sd->entity)
		{
			for (int i = 0; i < ed.total; ++i)
			{
				m_eFactory->createEntity(ed.name, ed.active, ed.layer, ed.x, ed.y);
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

			for (int i = 0; i < nse.tilemap.size(); ++i)
			{
				int tVal = nse.tilemap[i];
				if (std::find(std::begin(sd->tilesetData->staticTile),
					std::end(sd->tilesetData->staticTile), tVal) != std::end(sd->tilesetData->staticTile))
				{
					int mapX = nse.tilemapSize.x;
					int mapY = nse.tilemapSize.y;
					int tileX = nse.tilesetData->tileSize.x;
					int tileY = nse.tilesetData->tileSize.y;
					int x = (i % mapX) * tileX;
					int y = (i / mapX) * tileY;
					
					e->addComponent<PhysicsComponent>(e, (float)x, (float)y,
						(float)tileX, (float)tileY, 0.0f, 0.0f, 0.0f, 0.0f, true, true, false);
				}
			}

		}
		else sd->tilesetData = nullptr;

		nse.sceneSize = sd->sceneSize;
		nse.cellSize = sd->cellSize;

		// Broadcast NewSceneEvent

		broadcast(&nse);

		std::cout << name << " built successfully.\n";
	}

	void SceneFactory::readSceneData()
	{
		std::ifstream ifs{ "Data/Scenes.dat" };
		if (!ifs)
		{
			std::cerr << "WARNING: Scene data not found.\n";
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
		auto t = nextToken(ist);
		if (t == " ") return false;

		sd.name = t;

		t = nextToken(ist);
		if (t != "{")
		{
			std::cerr << "WARNING: Bad scene formatting.\n";
			return false;
		}

		while (true)
		{
			t = nextToken(ist);
			if (t == "Layer")
			{
				// Read Layer data

				LayerData ld;

				t = nextToken(ist);
				if (t != "{")
				{
					std::cerr << "WARNING: Bad layer formatting.\n";
					return false;
				}

				t = nextToken(ist);
				ld.name = t;
				t = nextToken(ist);
				if (t != ",")
				{
					std::cerr << "WARNING: Bad layer formatting.\n";
					return false;
				}
				t = nextToken(ist);
				ld.isStatic = (t == "static" ? true : false);
				t = nextToken(ist);
				if (t != "}")
				{
					std::cerr << "WARNING: Bad layer formatting.\n";
					return false;
				}
				sd.layer.push_back(ld);
			}
			else if (t == "Entity")
			{
				// Read entity data

				EntityData ed;
				t = nextToken(ist);
				if (t != "{")
				{
					std::cerr << "WARNING: Bad entity formatting.\n";
					return false;
				}
				t = nextToken(ist);
				ed.name = t;
				t = nextToken(ist);
				t = nextToken(ist);
				ed.total = std::stoi(t);
				t = nextToken(ist);
				t = nextToken(ist);
				ed.active = (t == "true" ? true : false);
				t = nextToken(ist);
				if (t == "}")
				{
					ed.layer = "default";
					ed.x = 0.0f;
					ed.y = 0.0f;
					sd.entity.push_back(ed);
					continue;
				}
				t = nextToken(ist);
				ed.layer = t;
				t = nextToken(ist);
				if (t == "}")
				{
					ed.x = 0.0f;
					ed.y = 0.0f;
					sd.entity.push_back(ed);
					continue;
				}
				t = nextToken(ist);
				ed.x = std::stof(t);
				t = nextToken(ist);
				t = nextToken(ist);
				ed.y = std::stof(t);
				sd.entity.push_back(ed);
				t = nextToken(ist);
			}
			else if (t == "Physics")
			{
				t = nextToken(ist);
				t = nextToken(ist);
				sd.sceneSize.x = std::stoi(t);
				t = nextToken(ist);
				t = nextToken(ist);
				sd.sceneSize.y = std::stoi(t);
				t = nextToken(ist);
				t = nextToken(ist);
				sd.cellSize.x = std::stoi(t);
				t = nextToken(ist);
				t = nextToken(ist);
				sd.cellSize.y = std::stoi(t);
				t = nextToken(ist);
			}
			else if (t == "Tilemap")
			{
				nextToken(ist);
				std::string tmName = nextToken(ist);

				auto iter = std::find_if(std::begin(m_tilesetData), std::end(m_tilesetData), [&](const TilesetData &td)
				{
					return td.name == tmName;
				});

				if (iter == std::end(m_tilesetData))
				{
					std::cerr << "WARNING: Tileset not found.\n";
					return false;
				}

				sd.tilesetData = &*iter;
				t = nextToken(ist);
				sd.tilemapLayer = nextToken(ist);
				nextToken(ist);
				sd.tilemapSize.x = std::stoi(nextToken(ist));
				nextToken(ist);
				sd.tilemapSize.y = std::stoi(nextToken(ist));

				int sz = sd.tilemapSize.x * sd.tilemapSize.y;
				for (int i = 0; i < sz; ++i)
				{
					nextToken(ist);
					sd.tilemap.push_back(std::stoi(nextToken(ist)));
				}

				if (nextToken(ist) != "}")
				{
					std::cerr << "WARNING: Bad tilemap formatting.\n";
					return false;
				}
			}
			else if (t != "}")
			{
				std::cerr << "WARNING: Bad scene formatting.\n";
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
			std::cerr << "WARNING: Tileset data not found.\n";
			return;
		}

		while (true)
		{
			auto t = nextToken(ifs);

			if (t == " ") break;
			
			TilesetData td;
			td.name = t;
			nextToken(ifs);
			td.texture = nextToken(ifs);
			nextToken(ifs);
			int x = std::stoi(nextToken(ifs));
			nextToken(ifs);
			int y = std::stoi(nextToken(ifs));
			td.textureSize = { x, y };

			nextToken(ifs);
			int tx = std::stoi(nextToken(ifs));
			nextToken(ifs);
			int ty = std::stoi(nextToken(ifs));
			td.tileSize = { tx, ty };
			t = nextToken(ifs);
			if (t != "}")
			{
				do
				{
					td.staticTile.push_back(std::stoi(nextToken(ifs)));
					t = nextToken(ifs);
				} while (t != "}");
			}
			m_tilesetData.push_back(td);
		}
	}

	std::string SceneFactory::nextToken(std::istream &ist)
	{
		std::string token;
		char c = ' ';
		char type;
		bool quotes{ false };

		// Skip whitespace and newline...
		while (' ' == c || '\n' == c)
		{
			ist.get(c);
			if (!ist)
			{
				token = " ";
				return token;
			}
		}

		// Deduce token type

		if (isalpha(c)) type = 's';
		else if (c == '\"')
		{
			type = 's';
			quotes = true;
		}
		else if (isdigit(c) || c == '-' || c == '.') type = '#';
		else
		{
			token += c;
			return token;
		}
		if (c != '\"')
			token += c;

		while (true)
		{
			ist.get(c);
			if (!ist)
			{
				return token;
			}

			if ('s' == type)
			{
				if (quotes)
				{
					if (c == '\"')
					{
						break;
					}
					token += c;
				}
				else
				{
					if (isalnum(c)) token += c;
					else
					{
						ist.putback(c);
						break;
					}
				}
			}
			else if ('#' == type)
			{
				if (isdigit(c) || c == '.') token += c;
				else
				{
					ist.putback(c);
					if (token[0] == '.') token = "0" + token;
					else if (token[0] == '-' && token[1] == '.') token.insert(std::begin(token) + 1, '0');
					break;
				}
			}
		}
		return token;
	}
}