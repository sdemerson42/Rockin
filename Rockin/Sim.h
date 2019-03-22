#pragma once

#include <vector>
#include <memory>
#include "CoreEntity.h"
#include "sde.h"
#include "SFML\Graphics.hpp"
#include "angelscript.h"
#include "EntityFactory.h"
#include "SceneFactory.h"

namespace Core
{
	class ScriptComponent;

	class Sim
	{
	public:
		Sim();
		~Sim();
		void execute();
		ScriptComponent *spawn(const std::string &tag);
	private:
		void systemsSetup();
		void scriptEngineSetup();
		void compileScripts();

		std::vector<std::unique_ptr<CoreEntity>> m_entity;
		std::vector<std::unique_ptr<sde::ISystem>> m_system;
		std::unique_ptr<EntityFactory> m_entityFactory;
		std::unique_ptr<SceneFactory> m_sceneFactory;
		sf::RenderWindow m_window;
		asIScriptEngine *m_scriptEngine;
	};
}
