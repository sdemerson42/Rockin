#include "Sim.h"
#include "Renderer.h"
#include "Physics.h"
#include "ScriptSystem.h"
#include "InputSystem.h"
#include "SoundSystem.h"
#include "Animator.h"

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"

#include "../../add_on/scriptbuilder/scriptbuilder.h"
#include "../../add_on/scriptstdstring/scriptstdstring.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <typeindex>

namespace Core
{
	Sim::Sim(unsigned int w, unsigned int h, const std::string &name) :
		m_window{ sf::VideoMode{w, h}, name }
	{

		// Create Systems

		systemsSetup();

		// Create and prepare script engine

		scriptEngineSetup();

		// Compile scripts

		compileScripts();

		// Create and prepare EntityFactory

		m_entityFactory = std::make_unique<EntityFactory>(&m_entity, m_scriptEngine);
		m_entityFactory->loadBlueprintData("Data/Blueprints.dat");

		// Create and prepare SceneFactory

		m_sceneFactory = std::make_unique<SceneFactory>(m_entityFactory.get(), &m_entity);

		m_sceneFactory->buildScene();
	}

	Sim::~Sim()
	{
		m_scriptEngine->ShutDownAndRelease();
	}

	void Sim::systemsSetup()
	{
		m_system.push_back(std::make_unique<InputSystem>());
		m_system.push_back(std::make_unique<Physics>());
		m_system.push_back(std::make_unique<ScriptSystem>());
		m_system.push_back(std::make_unique<SoundSystem>());
		m_system.push_back(std::make_unique<Animator>());
		m_system.push_back(std::make_unique<Renderer>(&m_window, 0.0f, 0.0f, m_window.getSize().x, m_window.getSize().y));
	}

	void Sim::scriptEngineSetup()
	{
		ScriptComponent::setSim(this);

		m_scriptEngine = asCreateScriptEngine();

		RegisterStdString(m_scriptEngine);

		m_scriptEngine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
		m_scriptEngine->RegisterObjectMethod("Entity", "bool hasTag(const string &in) const",
			asMETHOD(sde::Entity, hasTag), asCALL_THISCALL);

		m_scriptEngine->RegisterObjectType("Vector2f", 0, asOBJ_REF | asOBJ_NOCOUNT);
		m_scriptEngine->RegisterObjectProperty("Vector2f", "float x", asOFFSET(sf::Vector2f, x));
		m_scriptEngine->RegisterObjectProperty("Vector2f", "float y", asOFFSET(sf::Vector2f, y));

		m_scriptEngine->RegisterObjectType("InputEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
		m_scriptEngine->RegisterObjectProperty("InputEvent", "float stickX", asOFFSET(InputEvent, stickX));
		m_scriptEngine->RegisterObjectProperty("InputEvent", "float stickY", asOFFSET(InputEvent, stickY));
		m_scriptEngine->RegisterObjectProperty("InputEvent", "bool fire", asOFFSET(InputEvent, fire));

		m_scriptEngine->RegisterObjectType("ScriptComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void suspend(int cycles = 0)",
			asMETHOD(ScriptComponent, ScriptComponent::suspend), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void log(const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::log), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void setSleep(bool b)",
			asMETHOD(ScriptComponent, ScriptComponent::setSleep), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "bool entityHasTag(Entity @e, const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::entityHasTag), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "const Vector2f &position()",
			asMETHOD(ScriptComponent, ScriptComponent::position), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "const Vector2f &spriteSize()",
			asMETHOD(ScriptComponent, ScriptComponent::spriteSize), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void setPosition(float x, float y)",
			asMETHOD(ScriptComponent, ScriptComponent::setPosition), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "ScriptComponent @spawn(const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::spawn), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void despawn()",
			asMETHOD(ScriptComponent, ScriptComponent::despawn), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "const Vector2f &momentum()",
			asMETHOD(ScriptComponent, ScriptComponent::momentum), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void setMomentum(float x, float y)",
			asMETHOD(ScriptComponent, ScriptComponent::setMomentum), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "int randomRange(int min, int eMax)",
			asMETHOD(ScriptComponent, ScriptComponent::randomRange), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "const InputEvent &input()",
			asMETHOD(ScriptComponent, ScriptComponent::input), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void playSound(const string &in, float vol, bool loop, int priority)",
			asMETHOD(ScriptComponent, ScriptComponent::playSound), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "ScriptComponent @getScriptByTag(const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::getScriptByTag), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void setReg(const string &in, int val)",
			asMETHOD(ScriptComponent, ScriptComponent::setReg), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "int getReg(const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::getReg), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void modReg(const string &in, int val)",
			asMETHOD(ScriptComponent, ScriptComponent::modReg), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void playAnimation(const string &in)",
			asMETHOD(ScriptComponent, ScriptComponent::playAnimation), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void stopAnimation()",
			asMETHOD(ScriptComponent, ScriptComponent::stopAnimation), asCALL_THISCALL);
		m_scriptEngine->RegisterObjectMethod("ScriptComponent", "void setViewCenter(float x, float y)",
			asMETHOD(ScriptComponent, ScriptComponent::setViewCenter), asCALL_THISCALL);

	}

	void Sim::compileScripts()
	{
		CScriptBuilder builder;
		builder.StartNewModule(m_scriptEngine, "Behavior");

		std::ifstream ifs{ "Data/Scripts.dat" };
		while(true)
		{
			std::string fName;
			ifs >> fName;
			if (!ifs) break;
			builder.AddSectionFromFile(fName.c_str());
		}

		builder.BuildModule();
	}

	void Sim::execute()
	{
		const float framerate = 1000.0f / 60.0f;
		sf::Clock clock;

		while (m_window.isOpen())
		{
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
			}

			// Main loop

			if (clock.getElapsedTime().asMilliseconds() > framerate)
			{
				// Execute Systems

				for (auto &up : m_system)
					up->execute();

				clock.restart();
			}
		}
	}

	ScriptComponent *Sim::spawn(const std::string &tag)
	{
		ScriptComponent *r = nullptr;

		auto iter = std::find_if(std::begin(m_entity), std::end(m_entity), [&](std::unique_ptr<CoreEntity> &up)
		{
			return up->hasTag(tag) && !up->active();
		});

		if (iter != std::end(m_entity))
		{
			(*iter)->setActive(true);
			(*iter)->initializeAllComponents();
			r = (*iter)->getComponent<ScriptComponent>();
		}

		return r;
	}
}