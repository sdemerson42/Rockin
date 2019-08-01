#include <iostream>

#include "ScriptComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "Sim.h"
#include "CoreEntity.h"
#include "Events.h"
#include "Logger.h"

namespace Core
{
	Sim *ScriptComponent::m_sim;
	InputEvent ScriptComponent::m_input;

	void ScriptComponent::setSim(Sim *sim)
	{
		m_sim = sim;
	}

	void ScriptComponent::setInput(const InputEvent *event)
	{
		m_input = *event;
	}

	ScriptComponent::ScriptComponent(CoreEntity *parent, asIScriptEngine *engine, const std::string &tag) :
		CoreComponentBase{ parent }, m_engine{ engine }, m_sleep{ false }, m_suspensionCycles{ 0 }, m_prefix{ tag }
	{
		// Prepare scripting functions and contexts

		auto mod = engine->GetModule("Behavior");
		
		std::string sMain = std::string{ "void " + tag + "_main(ScriptComponent @p)" };
		const char *cMain = sMain.c_str();
		std::string sCollision = std::string{ "void " + tag + "_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)" };
		const char *cCollision = sCollision.c_str();

		m_funcMain = mod->GetFunctionByDecl(cMain);
		m_funcCollision = mod->GetFunctionByDecl(cCollision);

		m_contextMain = engine->CreateContext();
		m_contextMain->Prepare(m_funcMain);
		m_contextMain->SetArgObject(0, this);

		m_contextEvent = engine->CreateContext();
	}

	ScriptComponent::~ScriptComponent()
	{
		/*m_contextMain->Abort();
		m_contextMain->Release();
		m_contextEvent->Abort();
		m_contextEvent->Release();
		m_funcMain->Release();
		m_funcCollision->Release();*/
	}

	void ScriptComponent::initialize()
	{
		if (parent()->hasTag("Debris"))
			int x = 0;

		m_sleep = false;
		m_suspensionCycles = 0;
		m_registers.clear();
		m_strings.clear();

		auto mainState = m_contextMain->GetState();
		if (mainState == asEXECUTION_SUSPENDED || mainState == asEXECUTION_ABORTED) m_contextMain->Abort();
		m_contextMain->Prepare(m_funcMain);
		m_contextMain->SetArgObject(0, this);
	}

	const std::string &ScriptComponent::prefix() const
	{
		return m_prefix;
	}

	void ScriptComponent::addRegValue(const std::string &name, int val)
	{
		m_registers[name] = val;
	}

	void ScriptComponent::addStringValue(int index, const std::string &s)
	{
		m_strings[index] = s;
	}



	// Scripting API definitions



	void ScriptComponent::log(const std::string &msg)
	{
		Logger::log("Log request from ScriptComponent @" + std::to_string((unsigned long long)this) + ": " + msg);
	}

	void ScriptComponent::suspend(int cycles)
	{
		m_contextMain->Suspend();
		m_suspensionCycles = cycles;
	}

	void ScriptComponent::setSleep(bool b)
	{
		m_sleep = b;
	}

	bool ScriptComponent::entityHasTag(CoreEntity *e, const std::string &tag)
	{
		return e->hasTag(tag);
	}

	const sf::Vector2f &ScriptComponent::position()
	{
		return parent()->getComponent<TransformComponent>()->position();
	}

	void ScriptComponent::setPosition(float x, float y)
	{
		parent()->getComponent<TransformComponent>()->setPosition(x, y);
	}

	const sf::Vector2f &ScriptComponent::spriteSize()
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (rc) return rc->textureSize();
		return m_defaultVector2f;
	}

	ScriptComponent *ScriptComponent::spawn(const std::string &tag)
	{
		return m_sim->spawn(tag);
	}

	void ScriptComponent::despawn()
	{
		parent()->setActive(false);
	}

	const sf::Vector2f &ScriptComponent::momentum()
	{
		auto pc = parent()->getComponent<PhysicsComponent>();
		if (pc) return pc->momentum();
		return m_defaultVector2f;
	}

	void ScriptComponent::setMomentum(float x, float y)
	{
		auto pc = parent()->getComponent<PhysicsComponent>();
		if (pc) return pc->setMomentum(x, y);
	}

	int ScriptComponent::randomRange(int min, int eMax)
	{
		return rand() % (eMax - min) + min;
	}

	const InputEvent &ScriptComponent::input()
	{
		return m_input;
	}

	void ScriptComponent::playSound(const std::string &tag, float volume, bool loop, int priority)
	{
		SoundEvent se{ tag, volume, loop, priority };
		broadcast(&se);
	}

	void ScriptComponent::stopSound(const std::string &tag)
	{
		SoundEvent se{ tag, 0.0f, false, 0, true };
		broadcast(&se);
	}

	ScriptComponent *ScriptComponent::getScriptByTag(const std::string &tag)
	{
		auto sz = AutoList<ScriptComponent>::size();
		for (int i = 0; i < sz; ++i)
		{
			auto sc = AutoList<ScriptComponent>::get(i);
			if (sc->parent()->hasTag(tag))
				return sc;
		}
		return nullptr;
	}

	void ScriptComponent::playAnimation(const std::string &name)
	{
		auto ac = parent()->getComponent<AnimationComponent>();
		if (!ac) return;
		ac->play(name);
	}
	void ScriptComponent::stopAnimation()
	{
		auto ac = parent()->getComponent<AnimationComponent>();
		if (!ac) return;
		ac->stop();
	}

	void ScriptComponent::setViewCenter(float x, float y)
	{
		SetViewCenterEvent sve{ x,y };
		broadcast(&sve);
	}

	void ScriptComponent::setTextString(const std::string &s)
	{
		auto tc = parent()->getComponent<TextComponent>();
		if (tc) tc->setString(s);
	}

	void ScriptComponent::setTextSize(int val)
	{
		auto tc = parent()->getComponent<TextComponent>();
		if (tc) tc->setSize(val);
	}

	void ScriptComponent::setTextColor(int r, int g, int b, int a)
	{
		auto tc = parent()->getComponent<TextComponent>();
		if (tc) tc->setFillColor(r, g, b, a);
	}

	void ScriptComponent::changeScene(const std::string &scene)
	{
		ChangeSceneEvent cse;
		cse.sceneName = scene;
		cse.subscene = false;
		broadcast(&cse);
	}

	void ScriptComponent::changeSubscene(const std::string &scene)
	{
		ChangeSceneEvent cse;
		cse.sceneName = scene;
		cse.subscene = true;
		broadcast(&cse);
	}

	void ScriptComponent::setRenderFrame(float x, float y, float w, float h)
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (rc) rc->setFrame(x, y, w, h);
	}

	void ScriptComponent::playMusic(const std::string &fName, float volume, bool loop)
	{
		MusicEvent me;
		me.fileName = fName;
		me.volume = volume;
		me.loop = loop;
		me.action = 0;
		broadcast(&me);
	}

	void ScriptComponent::pauseMusic()
	{
		MusicEvent me;
		me.action = 1;
		broadcast(&me);
	}

	void ScriptComponent::stopMusic()
	{
		MusicEvent me;
		me.action = 2;
		broadcast(&me);
	}

	void ScriptComponent::setRenderScale(float x, float y)
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (rc) rc->setScale(x, y);
	}

	void ScriptComponent::setRenderRotation(float degrees)
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (rc) rc->setRotation(degrees);
	}

	void ScriptComponent::setRenderColor(int r, int g, int b, int a)
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (rc) rc->setColor(r, g, b, a);
	}

	void ScriptComponent::createSceneData(const std::string &name, int physWidth, int physHeight, int physCellWidth,
		int physCellHeight)
	{
		NewSceneDataEvent event;
		event.name = name;
		event.physWidth = physWidth;
		event.physHeight = physHeight;
		event.physCellWidth = physCellWidth;
		event.physCellHeight = physCellHeight;
		broadcast(&event);
	}

	void ScriptComponent::addSceneLayer(const std::string &sceneName, const std::string &layerName, bool isStatic)
	{
		AddSceneLayerEvent event;
		event.sceneName = sceneName;
		event.layerName = layerName;
		event.isStatic = isStatic;
		broadcast(&event);
	}
	
	void ScriptComponent::addSceneEntity(const std::string &sceneName, const std::string &entityName, int count, bool instantSpawn,
		const std::string &layer, float posX, float posY, bool persist, const CScriptArray &data)
	{
		AddSceneEntityEvent event;
		event.sceneName = sceneName;
		event.entityName = entityName;
		event.count = count;
		event.instantSpawn = instantSpawn;
		event.layer = layer;
		event.posX = posX;
		event.posY = posY;
		event.persist = persist;

		// Convert data from script array handles

		event.data.reserve(data.GetSize());
		for (int i = 0; i < data.GetSize(); ++i)
		{
			event.data.push_back(*static_cast<const std::string *>(data.At(i)));
		}

		broadcast(&event);
	}

	void ScriptComponent::addSceneTilemap(const std::string &sceneName, const std::string &tilesetName, const std::string &layer, int width, int height,
		const CScriptArray &tiles)
	{
		AddSceneTilemapEvent event;
		event.sceneName = sceneName;
		event.tilesetName = tilesetName;
		event.layer = layer;
		event.width = width;
		event.height = height;

		// Convert tiles data from script array handle

		event.tiles.reserve(tiles.GetSize());
		for (int i = 0; i < tiles.GetSize(); ++i)
		{
			event.tiles.push_back(*static_cast<const int *>(tiles.At(i)));
		}

		broadcast(&event);
	}

	void ScriptComponent::setReg(const std::string &reg, int val)
	{
		m_registers[reg] = val;
	}

	int ScriptComponent::getReg(const std::string &reg)
	{
		return m_registers[reg];
	}

	void ScriptComponent::modReg(const std::string &reg, int val)
	{
		m_registers[reg] += val;
	}

	void ScriptComponent::setString(int index, const std::string &s)
	{
		m_strings[index] = s;
	}

	const std::string &ScriptComponent::getString(int index)
	{
		return m_strings[index];
	}

	void ScriptComponent::setScript(const std::string &name, ScriptComponent *sc)
	{
		m_scripts[name] = sc;
	}

	ScriptComponent *ScriptComponent::getScript(const std::string &name)
	{
		return m_scripts[name];
	}
}