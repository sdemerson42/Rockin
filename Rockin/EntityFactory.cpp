#include "EntityFactory.h"
#include "CoreException.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "DataIO.h"
#include "Logger.h"

#include "Components.h"


#include "../../add_on/scriptbuilder/scriptbuilder.h"

namespace Core
{
	EntityFactory::EntityFactory(std::vector<std::unique_ptr<CoreEntity>> *eVec, asIScriptEngine *engine) :
		m_entity{ eVec }, m_engine{ engine }, m_currentEntity{ nullptr }
	{
	}

	void EntityFactory::createEntity(const std::string &name, bool active, bool persistent, const std::string &layer,
		float x, float y, const std::string &subsceneName)
	{
		// Find blueprint with provided name

		auto bpIter = std::find_if(std::begin(m_blueprint), std::end(m_blueprint), [&](const Blueprint &bp)
		{
			return bp.name == name;
		});

		if (bpIter == std::end(m_blueprint))
		{
			Logger::log("WARNING: EntityFactory failed to find entity blueprint with name " + name);
			return;
		}

		m_entity->push_back(std::make_unique<CoreEntity>());
		auto e = m_entity->back().get();

		e->setPosition(x, y);

		// Add components to Entity

		for (const std::string &tag : bpIter->tag) e->addTag(tag);
		for (const auto &data : bpIter->data)
		{
			if ("Physics" == data.component)
			{	
				e->addComponent<PhysicsComponent>(e, std::stof(data.valuePair[0].value), std::stof(data.valuePair[1].value),
					std::stof(data.valuePair[2].value), std::stof(data.valuePair[3].value), std::stof(data.valuePair[4].value),
					std::stof(data.valuePair[5].value), std::stof(data.valuePair[6].value), std::stof(data.valuePair[7].value),
					("true" == data.valuePair[8].value ? true : false), ("true" == data.valuePair[9].value ? true : false),
					("true" == data.valuePair[10].value ? true : false));
				
				auto cv = e->getComponents<PhysicsComponent>();
				auto alsp = cv[cv.size()-1];
				alsp->alsMoveRef(subsceneName);
			}
			else if ("PhysicsIgnore" == data.component)
			{
				auto pc = e->getComponent<PhysicsComponent>();
				if (pc)
				{
					for (const auto &vp : data.valuePair)
					{
						pc->addIgnoreTag(vp.value);
					}
				}
			}
			else if ("Render" == data.component)
			{
				e->addComponent<RenderComponent>(e, std::stof(data.valuePair[0].value), std::stof(data.valuePair[1].value),
					std::stof(data.valuePair[2].value), std::stof(data.valuePair[3].value),
					data.valuePair[4].value, layer);
				auto alsp = e->getComponent<RenderComponent>();
				alsp->alsMoveRef(subsceneName);
			}
			else if ("Script" == data.component)
			{
				e->addComponent<ScriptComponent>(e, m_engine, data.valuePair[0].value);
				auto alsp = e->getComponent<ScriptComponent>();
				alsp->alsMoveRef(subsceneName);
			}
			else if ("Animation" == data.component)
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
				a.loop = "true" == data.valuePair[2].value ? true : false;
				
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
				auto alsp = e->getComponent<AnimationComponent>();
				alsp->alsMoveRef(subsceneName);
			}
			else if ("Text" == data.component)
			{
				e->addComponent<TextComponent>(e, data.valuePair[0].value, data.valuePair[1].value,
					layer, std::stof(data.valuePair[2].value),
					std::stof(data.valuePair[3].value), std::stoi(data.valuePair[4].value),
					std::stoi(data.valuePair[5].value), std::stoi(data.valuePair[6].value),
					std::stoi(data.valuePair[7].value), std::stoi(data.valuePair[8].value));
			}
			else if ("Particles" == data.component)
			{
				e->addComponent<ParticlesComponent>(e, std::stof(data.valuePair[0].value),
					std::stof(data.valuePair[1].value), std::stoi(data.valuePair[2].value),
					std::stoi(data.valuePair[3].value), std::stoi(data.valuePair[4].value),
					std::stoi(data.valuePair[5].value), std::stoi(data.valuePair[6].value),
					std::stoi(data.valuePair[7].value), std::stof(data.valuePair[8].value),
					std::stof(data.valuePair[9].value), std::stoi(data.valuePair[10].value),
					std::stoi(data.valuePair[11].value), std::stoi(data.valuePair[12].value),
					("true" == data.valuePair[13].value ? true : false),
					std::stoi(data.valuePair[14].value), std::stoi(data.valuePair[15].value),
					std::stoi(data.valuePair[16].value), std::stoi(data.valuePair[17].value),
					layer);
			}
		}
		e->setActive(active);
		e->setPersistent(persistent);

		auto alsp = e->getComponent<TextComponent>();
		alsp->alsMoveRef(subsceneName);

		// For subsequent requests:

		m_currentEntity = e;
	}

	void EntityFactory::addInitData(const std::vector<std::string> &data)
	{
		// If initializing data is provided for a ScriptComponent
		// read and set values

		if (!m_currentEntity) return;
		auto sc = m_currentEntity->getComponent<ScriptComponent>();
		if (!sc) return;

		for (int i = 0; i < data.size(); i += 2)
		{
			std::string index = data[i];
			std::string val = data[i + 1];

			if (isalpha(index[0]))
			{
				sc->addRegValue(index, std::stoi(val));
			}
			if (isdigit(index[0]))
			{
				sc->addStringValue(std::stoi(index), val);
			}
		}
	}

	void EntityFactory::loadBlueprintData(const std::string &fName)
	{
		std::ifstream ifs{ fName };
		if (!ifs)
		{
			CoreException e{ "EntityFactory failed to open Blueprint data file.\n", CoreException::ErrCode::badBlueprintFile };
			throw(e);
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

	CoreEntity *EntityFactory::currentEntity() const
	{
		return m_currentEntity;
	}

	bool EntityFactory::readBlueprint(std::istream &ist, Blueprint &bp)
	{
		auto token = Tokenizer::next(ist);
		if (token == " ") return false;

		bp.name = token;

		token = Tokenizer::next(ist);
		if (!checkDataFormatting(token, "{", "WARNING: Bad blueprint formatting.")) return false;

		while (true)
		{
			token = Tokenizer::next(ist);
			if ("}" == token) break;

			// Read data

			// Tags

			if (token == "Tag")
			{
				token = Tokenizer::next(ist);
				if (!checkDataFormatting(token, "{", "WARNING: Bad tag data formatting.")) return false;

				while (true)
				{
					token = Tokenizer::next(ist);
					if ("," == token) continue;
					if ("}" == token || !ist) break;
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

				if ("Script" == cd.component)
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
		auto token = Tokenizer::next(ist);
		if (!checkDataFormatting(token, "{", "WARNING: Bad component data formatting.")) return false;

		while (true)
		{
			token = Tokenizer::next(ist);
			if ("}" == token) return true;
			if ("," == token) continue;

			// Fix floats with no digits before the point

			if (token.size() > 1)
			{
				if ('.' == token[0]) token = "0" + token;
				else if ('-' == token[0]  && '.' == token[1]) token.insert(std::begin(token) + 1, '0');
			}

			BPair pair;
			if (token[0] == '-' || isdigit(token[0]))
			{
				auto result = token.find('.');
				if (result != std::string::npos) pair.type = BType::Float;
				else pair.type = BType::Int;
			}
			else if ("true" == token || "false" == token) pair.type = BType::Bool;
			else pair.type = BType::String;

			pair.value = token;
			cd.valuePair.push_back(pair);
		}
	}
}