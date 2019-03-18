#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"

namespace Core
{
	class PhysicsComponent : public sde::ComponentBase, public sde::AutoList<PhysicsComponent>
	{
		friend class Physics;
	public:
		PhysicsComponent(sde::Entity *parent, float ox, float oy, float w, float h, float mx, float my,
			float mass, float elasticity, bool solid = true, bool stat = false, bool noCollision = false) :
			ComponentBase{ parent }, m_AABBOffset{ ox, oy }, m_AABBSize{ w, h },
			m_momentum{ mx, my }, m_mass{ mass }, m_elasticity{ elasticity }, m_solid{ solid }, m_static{ stat }, m_noCollision{ noCollision }
		{
			if (mass == 0.0f) m_inverseMass = 0.0f;
			else m_inverseMass = 1.0f / mass;
		}
		const sf::Vector2f &momentum() const
		{
			return m_momentum;
		}
		void setMomentum(float x, float y)
		{
			m_momentum = { x, y };
		}
	private:
		bool m_static;
		bool m_solid;
		bool m_noCollision;
		sf::Vector2f m_momentum;
		sf::Vector2f m_AABBOffset;
		sf::Vector2f m_AABBSize;
		float m_mass;
		float m_inverseMass;
		float m_elasticity;
	};
}
