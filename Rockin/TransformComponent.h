#pragma once

#include "CoreComponentBase.h"
#include "SFML\Graphics.hpp"

namespace Core
{
	class TransformComponent : public CoreComponentBase, public sde::AutoList<TransformComponent>
	{
	public:
		TransformComponent(CoreEntity *parent) :
			CoreComponentBase{ parent }, m_position { 0.0f, 0.0f }
		{}
		TransformComponent(CoreEntity *parent, float x, float y) :
			CoreComponentBase{ parent }, m_position{ x, y }
		{}
		const sf::Vector2f &position() const
		{
			return m_position;
		}
		void setPosition(float x, float y)
		{
			m_position.x = x;
			m_position.y = y;
		}
		void setPosition(const sf::Vector2f &pos)
		{
			m_position = pos;
		}
		void adjustPosition(float x, float y)
		{
			m_position.x += x;
			m_position.y += y;
		}
		void adjustPosition(const sf::Vector2f &pos)
		{
			m_position += pos;
		}
	private:
		sf::Vector2f m_position;
	};
}
