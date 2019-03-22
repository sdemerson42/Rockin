#include "SceneFactory.h"
#include <iostream>

namespace Core
{
	SceneFactory::SceneFactory(EntityFactory *eFactory) :
		m_eFactory{ eFactory }
	{
		readSceneData();
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
		else if (isdigit(c) || c == '-' || c == '.') type = '#';
		else
		{
			token += c;
			return token;
		}
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
				if (isalnum(c)) token += c;
				else
				{
					ist.putback(c);
					break;
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
};