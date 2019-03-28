#include "SceneFactory.h"
#include "CoreEntity.h"
#include <iostream>
#include "Events.h"
#include <algorithm>
#include "EntityFactory.h"

namespace Core
{
	SceneFactory::SceneFactory(EntityFactory *eFactory, std::vector<std::unique_ptr<CoreEntity>> *eVec) :
		m_eFactory{ eFactory }, m_eVec{ eVec }
	{
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
			else if (t == "}")
			{
				return true;
			}
			else
			{
				std::cerr << "WARNING: Bad scene formatting.\n";
				return false;
			}
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