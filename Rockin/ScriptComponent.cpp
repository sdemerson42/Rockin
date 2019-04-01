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
		std::string sCollision = std::string{ "void " + tag + "_onCollision(ScriptComponent @p, Entity @collider)" };
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

	// Script interface

	void ScriptComponent::log(const std::string &msg)
	{
		std::cout << '<' << this << ">: " << msg << std::endl;
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
		if (rc) return rc->tSize();
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
}