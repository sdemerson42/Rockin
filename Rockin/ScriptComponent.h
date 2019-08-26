#pragma once

#include "CoreComponentBase.h"
#include "angelscript.h"
#include <string>
#include "SFML\Graphics.hpp"
#include <iostream>
#include "Events.h"
#include <map>
#include "AutoListScene.h"
#include "../../add_on/scriptarray/scriptarray.h"

namespace Core
{
	class Sim;

	/*
	ScriptComponent - Contains state necessary for the correct execution of scripts as well as persistent
	state that scripts may access to cut down on use of global script variables. The scripting API is
	also located here to simplify calling syntax within individual scripts.
	*/

	class ScriptComponent : public CoreComponentBase, public sde::AutoList<ScriptComponent>, public AutoListScene<ScriptComponent>
	{
	public:
		ScriptComponent(CoreEntity *parent, asIScriptEngine *engine, const std::string &tag);
		~ScriptComponent();
		static void setSim(Sim *sim);
		static void setInput(const InputEvent *ie);
		void initialize() override;
		const std::string &prefix() const;
		void addRegValue(const std::string &name, int val);
		void addStringValue(int index, const std::string &s);
		void resetMainScriptFunction(const std::string &funcName);

		//Accessors
		bool sleep() const
		{
			return m_sleep;
		}
		int suspensionCycles() const
		{
			return m_suspensionCycles;
		}
		void decSuspensionCycles()
		{
			--m_suspensionCycles;
		}
		asIScriptContext *contextMain()
		{
			return m_contextMain;
		}
		asIScriptContext *contextEvent()
		{
			return m_contextEvent;
		}
		asIScriptFunction *collisionFunction()
		{
			return m_funcCollision;
		}

		// Script types
		static asITypeInfo *m_asTypeStringArray;

		// Scripting API

		void log(const std::string &msg);
		void suspend(int cycles);
		void setSleep(bool b);
		bool entityHasTag(CoreEntity *e, const std::string &tag);
		const sf::Vector2f &position();
		void setPosition(float x, float y);
		const sf::Vector2f &spriteSize();
		ScriptComponent *spawn(const std::string &tag);
		void despawn();
		const sf::Vector2f &momentum();
		void setMomentum(float x, float y);
		int randomRange(int min, int eMax);
		const InputEvent &input();
		void playSound(const std::string &tag, float volume, bool loop, int priority);
		ScriptComponent *getScriptByTag(const std::string &tag);
		void stopSound(const std::string &tag);
		void playAnimation(const std::string &name);
		void stopAnimation();
		void setViewCenter(float x, float y);
		void setTextString(const std::string &s);
		void setTextSize(int val);
		void setTextColor(int r, int g, int b, int a);
		void changeScene(const std::string &scene);
		void changeSubscene(const std::string &scene);
		void setRenderFrame(float x, float y, float w, float h);
		void playMusic(const std::string &fName, float volume, bool loop);
		void pauseMusic();
		void stopMusic();
		void setRenderScale(float x, float y);
		void setRenderRotation(float degrees);
		void setRenderColor(int r, int g, int b, int a);
		ScriptComponent *forceSpawn(const std::string &tag, const std::string &layer);
		void setColliderSize(float x, float y);
		void setColliderOffset(float x, float y);
		void setComponentActive(const std::string &compName, bool state);
		int tileAtPosition(float x, float y) const;

		void createSceneData(const std::string &name, int physWidth, int physHeight, int physCellWidth,
			int physCellHeight);
		void addSceneLayer(const std::string &sceneName, const std::string &layerName, bool isStatic);
		void addSceneEntity(const std::string &sceneName, const std::string &entityName, int count, bool instantSpawn,
			const std::string &layer, float posX, float posY, bool persist);
		void addSceneDataEntity(const std::string &sceneName, const std::string &entityName, int count, bool instantSpawn,
			const std::string &layer, float posX, float posY, bool persist, const CScriptArray &data);
		void addSceneTilemap(const std::string &sceneName, const std::string &tilesetName, const std::string &layer, int width, int height,
			const CScriptArray &tiles);
		void addSubscene(const std::string &sceneName, const std::string &subsceneName);
		void removeSceneEntity(const std::string &sceneName, const std::string &entityName);
		void addSceneBase(const std::string &sceneName, const std::string &baseName);

		CScriptArray *readDataFromFile(const std::string &fName);

		void setMainFunction(const std::string &funcName);

		void setReg(const std::string &reg, int value);
		int getReg(const std::string &reg);
		int modReg(const std::string &reg, int value);

		void setString(int index, const std::string &s);
		const std::string &getString(int index);

		void setScript(const std::string &name, ScriptComponent *sc);
		ScriptComponent *getScript(const std::string &name);
	private:
		asIScriptEngine *m_engine;
		asIScriptContext *m_contextMain;
		asIScriptContext *m_contextEvent;
		asIScriptFunction *m_funcMain;
		asIScriptFunction *m_funcCollision;
		static InputEvent m_input;
		static Sim *m_sim;
		std::string m_prefix;

		// Script state

		bool m_sleep;
		int m_suspensionCycles;
		std::map<std::string, int> m_registers;
		std::map<int, std::string> m_strings;
		std::map<std::string, ScriptComponent *> m_scripts;
		sf::Vector2f m_defaultVector2f;
	};
}
