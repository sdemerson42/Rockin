#include "ScriptSystem.h"
#include "ScriptComponent.h"

namespace Core
{
	ScriptSystem::ScriptSystem()
	{
		registerFunc(this, &ScriptSystem::onCollision);
	}

	void ScriptSystem::execute()
	{
		auto sz = AutoListScene<ScriptComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto sc = AutoListScene<ScriptComponent>::alsCurrentGet(i);
			if (sc->active() && !sc->m_sleep && sc->m_suspensionCycles <= 0) sc->m_contextMain->Execute();
			if (sc->m_suspensionCycles > 0) --sc->m_suspensionCycles;
		}
	}

	void ScriptSystem::onCollision(const CollisionEvent *c)
	{
		if (!c->receiver->active())
			return;

		auto context = c->receiver->m_contextEvent;
		context->Prepare(c->receiver->m_funcCollision);
		context->SetArgObject(0, c->receiver);
		context->SetArgObject(1, c->collider);
		context->SetArgObject(2, c->colliderPhysics);
		context->Execute();
	}
}