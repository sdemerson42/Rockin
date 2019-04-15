#pragma once

#include "CoreComponentBase.h"
#include "SFML\Graphics.hpp"
#include "AutoListScene.h"

namespace Core
{
	class PhysicsComponent : public CoreComponentBase, public sde::AutoList<PhysicsComponent>, public AutoListScene<PhysicsComponent>
	{
		friend class Physics;
	public:
		PhysicsComponent(CoreEntity *parent, float ox, float oy, float w, float h, float mx, float my,
			float mass, float elasticity, bool solid = true, bool stat = false, bool noCollision = false) :
			CoreComponentBase{ parent }, m_AABBOffset{ ox, oy }, m_AABBSize{ w, h },
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
		bool solid()
		{
			return m_solid;
		}
		void setSolid(bool val)
		{
			m_solid = val;
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
