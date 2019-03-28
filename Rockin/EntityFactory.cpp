#include "EntityFactory.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "TextComponent.h"

#include "../../add_on/scriptbuilder/scriptbuilder.h"

namespace Core
{
	EntityFactory::EntityFactory(std::vector<std::unique_ptr<CoreEntity>> *eVec, asIScriptEngine *engine) :
		m_entity{ eVec }, m_engine{ engine }
	{
	}

	void EntityFactory::createEntity(const std::string &name, bool active, const std::string &layer,
		float x, float y)
	{
		auto bpIter = std::find_if(std::begin(m_blueprint), std::end(m_blueprint), [&](const Blueprint &bp)
		{
			return bp.name == name;
		});

		if (bpIter == std::end(m_blueprint)) return;

		m_entity->push_back(std::make_unique<CoreEntity>());
		auto e = m_entity->back().get();

		e->setPosition(x, y);

		for (const std::string &tag : bpIter->tag) e->addTag(tag);
		for (const auto &data : bpIter->data)
		{
			// Construct components

			/*if (data.component == "Transform")
			{
				e->addComponent<TransformComponent>(e, std::stof(data.valuePair[0].value),
					std::stof(data.valuePair[1].value));
			}*/
			if (data.component == "Physics")
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
					std::stof(data.valuePair[2].value), std::stof(data.valuePair[3].value),
					data.valuePair[4].value, layer);
			}
			else if (data.component == "Script")
			{
				e->addComponent<ScriptComponent>(e, m_engine, data.valuePair[0].value);
			}
			else if (data.component == "Animation")
			{
				auto ac = e->getComponent<AnimationComponent>();
				if (!ac)
				{
					e->addComponent<AnimationComponent>(e);
					ac = e->getComponent<AnimationComponent>();
				}
				AnimationComponent::Animation a;
				a.name = data.valuePair[0].value;
				a.frameDelay = std::stoi(data.valuePair[1].value);
				a.loop = data.valuePair[2].value == "true" ? true : false;
				
				int totalFrames = std::stoi(data.valuePair[3].value);
				int j = 4;
				for (int i = 0; i < totalFrames; ++i)
				{
					AnimationComponent::Frame frame;
					frame.position.x = std::stof(data.valuePair[j++].value);
					frame.position.y = std::stof(data.valuePair[j++].value);
					frame.size.x = std::stof(data.valuePair[j++].value);
					frame.size.y = std::stof(data.valuePair[j++].value);
					a.frame.push_back(frame);
				}
				ac->addAnimation(a);
			}
			else if (data.component == "Text")
			{
				e->addComponent<TextComponent>(e, data.valuePair[0].value, data.valuePair[1].value,
					layer, std::stof(data.valuePair[2].value),
					std::stof(data.valuePair[3].value), std::stoi(data.valuePair[4].value),
					std::stoi(data.valuePair[5].value), std::stoi(data.valuePair[6].value),
					std::stoi(data.valuePair[7].value), std::stoi(data.valuePair[8].value));
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

		// Compile scripts

		CScriptBuilder builder;
		builder.StartNewModule(m_engine, "Behavior");

		for (const auto &fName : m_compiledScripts)
		{
			builder.AddSectionFromFile(fName.c_str());
		}

		builder.BuildModule();
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

				// Add script path if necessary

				if (cd.component == "Script")
				{
					if (std::find(std::begin(m_compiledScripts), std::end(m_compiledScripts), cd.valuePair[1].value) == std::end(m_compiledScripts))
						m_compiledScripts.push_back(cd.valuePair[1].value);
				}
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