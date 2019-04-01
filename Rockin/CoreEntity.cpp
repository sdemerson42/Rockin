#include "CoreEntity.h"
#include "TransformComponent.h"
#include "ScriptComponent.h"

namespace Core
{
	CoreEntity::CoreEntity()
	{
		addComponent<TransformComponent>(this);
	}

	bool CoreEntity::persistent() const
	{
		return m_persistent;
	}

	void CoreEntity::setPersistent(bool b)
	{
		m_persistent = b;
	}

	const sf::Vector2f &CoreEntity::position()
	{
		return getComponent<TransformComponent>()->position();
	}

	void CoreEntity::setPosition(const sf::Vector2f &v)
	{
		getComponent<TransformComponent>()->setPosition(v);
	}

	void CoreEntity::setPosition(float x, float y)
	{
		getComponent<TransformComponent>()->setPosition(x, y);
	}

	void CoreEntity::adjustPosition(const sf::Vector2f &v)
	{
		getComponent<TransformComponent>()->setPosition(v);
	}

	void CoreEntity::adjustPosition(float x, float y)
	{
		getComponent<TransformComponent>()->setPosition(x, y);
	}

	ScriptComponent *CoreEntity::scriptComponent()
	{
		auto sc = getComponent<ScriptComponent>();
		if (sc) return sc;
		return nullptr;
	}
}