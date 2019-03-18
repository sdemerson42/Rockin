#pragma once

#include "sde.h"
#include "angelscript.h"
#include <string>
#include "SFML\Graphics.hpp"
#include <iostream>
#include "Events.h"
#include <map>

namespace Core
{

	class ScriptComponent : public sde::ComponentBase, public sde::AutoList<ScriptComponent>
	{
		friend class ScriptSystem;
		friend class Sim;
	public:
		ScriptComponent(sde::Entity *parent, asIScriptEngine *engine, const std::string &tag);
		~ScriptComponent();
		static void setSim(Sim *);
		static void setInput(const InputEvent *);
		void initialize() override;
		const std::string &prefix() const;
	private:
		asIScriptEngine *m_engine;
		asIScriptContext *m_contextMain;
		asIScriptContext *m_contextEvent;
		asIScriptFunction *m_funcMain;
		asIScriptFunction *m_funcCollision;
		static InputEvent m_input;
		static Sim *m_sim;
		std::string m_prefix;

		// Script interface

		void log(const std::string &msg);
		void suspend(int cycles);
		void setSleep(bool b);
		bool entityHasTag(sde::Entity *e, const std::string &tag);
		const sf::Vector2f &position();
		void setPosition(float x, float y);
		ScriptComponent *spawn(const std::string &tag);
		void despawn();
		const sf::Vector2f &momentum();
		void setMomentum(float x, float y);
		int randomRange(int min, int eMax);
		const InputEvent &input();
		void playSound(const std::string &tag, float volume, bool loop, int priority);
		ScriptComponent *getScriptByTag(const std::string &tag);

		void setReg(const std::string &reg, int value);
		int getReg(const std::string &reg);
		void modReg(const std::string &reg, int value);

		// Script state

		bool m_sleep;
		int m_suspensionCycles;
		std::map<std::string, int> m_registers;
	};
}
