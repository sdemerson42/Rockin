#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CoreEntity.h"
#include "sde.h"
#include "SFML\Graphics.hpp"
#include "angelscript.h"
#include "EntityFactory.h"
#include "SceneFactory.h"
#include "Events.h"

namespace Core
{
	class ScriptComponent;
	
	/*
	Sim - Primary simulation class. Owns all Systems and Entities.
	On construction the Sim creates and initializes Systems, creates the
	AngelScript engine, registers the scripting API, and prepares
	the starting scene. The execute() method contains the main game loop.
	*/

	class Sim : public sde::EventHandler
	{
	public:
		Sim(unsigned int w, unsigned int h, const std::string &name);
		~Sim();
		Sim(const Sim &) = delete;
		Sim(Sim &&) = delete;
		Sim &operator=(const Sim &) = delete;
		Sim &operator=(Sim &&) = delete;
		void execute();
		ScriptComponent *spawn(const std::string &tag);
	private:
		void systemsSetup();
		void scriptEngineSetup();

		std::vector<std::unique_ptr<CoreEntity>> m_entity;
		std::vector<std::unique_ptr<sde::ISystem>> m_system;
		std::unique_ptr<EntityFactory> m_entityFactory;
		std::unique_ptr<SceneFactory> m_sceneFactory;
		sf::RenderWindow m_window;
		asIScriptEngine *m_scriptEngine;
		std::string m_nextScene;
		bool m_subsceneChange;

		void onChangeScene(const ChangeSceneEvent *cse);
	};
}
