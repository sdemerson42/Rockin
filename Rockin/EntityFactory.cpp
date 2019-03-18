#include "EntityFactory.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "ScriptComponent.h"

namespace Core
{
	EntityFactory::EntityFactory(std::vector<std::unique_ptr<sde::Entity>> *eVec, asIScriptEngine *engine) :
		m_entity{ eVec }, m_engine{ engine }
	{
	}

	void EntityFactory::createEntity(const std::string &name, bool active)
	{
		auto bpIter = std::find_if(std::begin(m_blueprint), std::end(m_blueprint), [&](const Blueprint &bp)
		{
			return bp.name == name;
		});

		if (bpIter == std::end(m_blueprint)) return;

		m_entity->push_back(std::make_unique<sde::Entity>());
		sde::Entity *e = m_entity->back().get();

		for (const std::string &tag : bpIter->tag) e->addTag(tag);
		for (const auto &data : bpIter->data)
		{
			// Construct components

			if (data.component == "Transform")
			{
				e->addComponent<TransformComponent>(e, std::stof(data.valuePair[0].value),
					std::stof(data.valuePair[1].value));
			}
			else if (data.component == "Physics")
			{	
				e->addComponent<PhysicsComponent>(e, std::stof(data.valuePair[0].value), std::stof(data.valuePair[1].value),
					std::stof(data.valuePair[2].value), std::stof(data.valuePair[3].value), std::stof(data.valuePair[4].value),
					std::stof(data.valuePair[5].value), std::stof(data.valuePair[6].value), std::stof(data.valuePair[7].value),
					(data.valuePair[8].value == "true" ? true : false), (data.valuePair[9].value == "true" ? true : false),
					(data.valuePair[10].value == "true" ? true : false));
			}
			else if (data.component == "Render")
			{
				e->addComponent<RenderComponent>(e, std::stof(data.valuePair[0].value), std::stof(data.valuePair[1].value),
					std::stof(data.valuePair[2].value), std::stof(data.valuePair[3].value));
			}
			else if (data.component == "Script")
			{
				e->addComponent<ScriptComponent>(e, m_engine, data.valuePair[0].value);
			}
		}
		e->setActive(active);
	}

	void EntityFactory::loadBlueprintData(const std::string &fName)
	{
		std::ifstream ifs{ fName };
		if (!ifs)
		{
			std::cerr << "WARNING: EntityFactory failed to open Blueprint data file.\n";
			return;
		}

		while (true)
		{
			Blueprint bp;
			if (!readBlueprint(ifs, bp))
				break;
			m_blueprint.push_back(bp);
		}
	}

	bool EntityFactory::readBlueprint(std::istream &ist, Blueprint &bp)
	{
		auto token = nextToken(ist);
		if (token == " ") return false;

		bp.name = token;

		token = nextToken(ist);
		if (token != "{")
		{
			std::cerr << "WARNING: Bad blueprint formatting.\n";
			return false;
		}

		while (true)
		{
			token = nextToken(ist);
			if (token == "}") break;

			// Read data

			// Tags

			if (token == "Tag")
			{
				token = nextToken(ist);
				if (token != "{")
				{
					std::cerr << "WARNING: Bad tag data formatting.\n";
					return false;
				}

				while (true)
				{
					token = nextToken(ist);
					if (token == ",") continue;
					if (token == "}") break;
					bp.tag.push_back(token);
				}
			}

			// Components

			else
			{
				ComponentData cd;
				cd.component = token;
				if (readComponentData(ist, cd)) bp.data.push_back(cd);
			}
		}

		return true;
	}

	bool EntityFactory::readComponentData(std::istream &ist, ComponentData &cd)
	{
		auto token = nextToken(ist);
		if (token != "{")
		{
			std::cerr << "WARNING: Bad component data formatting.\n";
			return false;
		}

		while (true)
		{
			token = nextToken(ist);
			if (token == "}") return true;
			if (token == ",") continue;

			// Fix floats with no digits before the point

			if (token.size() > 1)
			{
				if (token[0] == '.') token = "0" + token;
				else if (token[0] == '-' && token[1] == '.') token.insert(std::begin(token) + 1, '0');
			}

			BPair pair;
			if (token[0] == '-' || isdigit(token[0]))
			{
				auto result = token.find('.');
				if (result != std::string::npos) pair.type = BType::Float;
				else pair.type = BType::Int;
			}
			else if (token == "true" || token == "false") pair.type = BType::Bool;
			else pair.type = BType::String;

			pair.value = token;
			cd.valuePair.push_back(pair);
		}
	}

	std::string EntityFactory::nextToken(std::istream &ist)
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
					break;
				}
			}
		}
		return token;
	}
}