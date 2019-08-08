#pragma once

#include "CoreComponentBase.h"
#include "SFML\Graphics.hpp"
#include "AutoListScene.h"

namespace Core
{
	/*
	PhysicsComponent - Component that holds all data necessary to resolve collisions.
	Calculations are processed by the Physics system.
	*/
	class PhysicsComponent : public CoreComponentBase, public sde::AutoList<PhysicsComponent>, public AutoListScene<PhysicsComponent>
	{
	public:
		PhysicsComponent(CoreEntity *parent, float ox, float oy, float w, float h, float mx, float my,
			float mass, float elasticity, bool solid = true, bool stat = false, bool noCollision = false) :
			CoreComponentBase{ parent }, m_AABBOffset{ ox, oy }, m_AABBSize{ w, h },
			m_momentum{ mx, my }, m_mass{ mass }, m_elasticity{ elasticity }, m_solid{ solid }, m_static{ stat }, m_noCollision{ noCollision }
		{
			if (mass == 0.0f) m_inverseMass = 0.0f;
			else m_inverseMass = 1.0f / mass;
		}
		// Accessors
		const sf::Vector2f &momentum() const
		{
			return m_momentum;
		}
		void setMomentum(float x, float y)
		{
			m_momentum = { x, y };
		}
		void setMomentum(const sf::Vector2f &vec)
		{
			m_momentum = vec;
		}
		bool solid() const
		{
			return m_solid;
		}
		void setSolid(bool val)
		{
			m_solid = val;
		}
		bool getStatic() const
		{
			return m_static;
		}
		void setStatic(bool val)
		{
			m_static = val;
		}
		bool noCollision() const
		{
			return m_noCollision;
		}
		void setNoCollision(bool val)
		{
			m_noCollision = val;
		}
		const sf::Vector2f &boundingBoxOffset() const
		{
			return m_AABBOffset;
		}
		const sf::Vector2f &boundingBoxSize() const
		{
			return m_AABBSize;
		}
		void setBoundingBoxOffset(float x, float y)
		{
			m_AABBOffset = { x, y };
		}
		void setBoundingBoxSize(float x, float y)
		{
			m_AABBSize = { x, y };
		}
		float mass()
		{
			return m_mass;
		}
		float iMass()
		{
			return m_inverseMass;
		}
		float elasticity()
		{
			return m_elasticity;
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
