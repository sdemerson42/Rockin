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
			if (sc->active() && !sc->sleep() && sc->suspensionCycles() <= 0) sc->contextMain()->Execute();
			if (sc->suspensionCycles() > 0) sc->decSuspensionCycles();
		}
	}

	void ScriptSystem::onCollision(const CollisionEvent *c)
	{
		if (!c->receiver->active())
			return;

		auto context = c->receiver->contextEvent();
		context->Prepare(c->receiver->collisionFunction());
		context->SetArgObject(0, c->receiver);
		context->SetArgObject(1, c->collider);
		context->SetArgObject(2, c->colliderPhysics);
		context->Execute();
	}
}