#include "Physics.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "ScriptComponent.h"
#include "Events.h"
#include <algorithm>

namespace Core
{
	
	void Physics::execute()
	{
		applyMomentum();
		resolveCollisions();
	}

	void Physics::applyMomentum()
	{
		auto sz = sde::AutoList<PhysicsComponent>::size();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = sde::AutoList<PhysicsComponent>::get(i);
			if (!pc->active() || pc->m_static) continue;
			auto tc = pc->parent()->getComponent<TransformComponent>();
			if (!tc) continue;
			float x = tc->position().x + pc->m_momentum.x;
			float y = tc->position().y + pc->m_momentum.y;
			tc->setPosition(x, y);
		}
	}

	void Physics::resolveCollisions()
	{
		auto sz = sde::AutoList<PhysicsComponent>::size();
		for (size_t i = 0; i < sz; ++i)
		{
			auto a = sde::AutoList<PhysicsComponent>::get(i);
			if (!a->active()) continue;
			for (size_t k = i + 1; k < sz; ++k)
			{
				auto b = sde::AutoList<PhysicsComponent>::get(k);
				if (!b->active()) continue;

				// Escape conditions

				if (a->m_momentum.x == 0.0f && a->m_momentum.y == 0.0f &&
					b->m_momentum.x == 0.0f && b->m_momentum.y == 0.0f)
					continue;

				if (a->m_inverseMass == 0.0f && b->m_inverseMass == 0.0f) continue;

				if (a->m_noCollision || b->m_noCollision) continue;

				Collision collision{ a, b };
				if (detectCollision(&collision))
				{
					if (a->m_solid && b->m_solid)
					{
						adjustMomentum(&collision);
						
						// Adjust position if static objects are involved

						if (a->m_static || b->m_static) applyCorrection(&collision);
					}
					
					// Alert scripts if applicable

					auto ae = a->parent();
					auto asc = ae->getComponent<ScriptComponent>();
					auto be = b->parent();
					auto bsc = be->getComponent<ScriptComponent>();

					if (asc)
					{
						CollisionEvent ce;
						ce.collider = be;
						ce.receiver = asc;
						broadcast(&ce);
					}
					if (bsc)
					{
						CollisionEvent ce;
						ce.collider = ae;
						ce.receiver = bsc;
						broadcast(&ce);
					}
				}
			}
		}
	}

	void Physics::adjustMomentum(Collision *collision)
	{
		auto a = collision->a;
		auto b = collision->b;

		float e = (a->m_elasticity + b->m_elasticity) / 2.0f;
		auto rv = sf::Vector2f{ b->m_momentum.x - a->m_momentum.x, b->m_momentum.y - a->m_momentum.y };
		float velAlongNormal = rv.x * collision->normal.x + rv.y * collision->normal.y;
		if (velAlongNormal > 0.0f) return;
		float j = (-1.0f*(e + 1.0f) * velAlongNormal) / (a->m_inverseMass + b->m_inverseMass);

		a->m_momentum -= j * collision->normal * a->m_inverseMass;
		b->m_momentum += j * collision->normal * b->m_inverseMass;
	}

	void Physics::applyCorrection(Collision *collision)
	{
		const float slop = 0.1f;
		const float percCorrect = 0.8f;

		auto ta = collision->a->parent()->getComponent<TransformComponent>();
		auto tb = collision->b->parent()->getComponent<TransformComponent>();

		sf::Vector2f correction = std::max(collision->penetration - slop, 0.0f) / 
			(collision->a->m_inverseMass + collision->b->m_inverseMass) * percCorrect * collision->normal;
		ta->adjustPosition(-1.0f * collision->a->m_inverseMass * correction);
		tb->adjustPosition(collision->b->m_inverseMass * correction);
	}

	bool Physics::detectCollision(Collision *c)
	{
		auto atc = c->a->parent()->getComponent<TransformComponent>();
		if (!atc) return false;
		auto btc = c->b->parent()->getComponent<TransformComponent>();
		if (!btc) return false;

		sf::Vector2f aBoxPos{ atc->position().x + c->a->m_AABBOffset.x, atc->position().y + c->a->m_AABBOffset.y };
		sf::Vector2f bBoxPos{ btc->position().x + c->b->m_AABBOffset.x, btc->position().y + c->b->m_AABBOffset.y };
		sf::Vector2f aSize{ c->a->m_AABBSize };
		sf::Vector2f bSize{ c->b->m_AABBSize };
		sf::Vector2f aCenter{ aBoxPos.x + aSize.x / 2.0f, aBoxPos.y + aSize.y / 2.0f };
		sf::Vector2f bCenter{ bBoxPos.x + bSize.x / 2.0f, bBoxPos.y + bSize.y / 2.0f };
		sf::Vector2f extent{ bCenter.x - aCenter.x, bCenter.y - aCenter.y };

		float xOverlap = (aSize.x / 2.0f + bSize.x / 2.0f) - abs(extent.x);
		if (xOverlap > 0.0f)
		{
			float yOverlap = (aSize.y / 2.0f + bSize.y / 2.0f) - abs(extent.y);
			if (yOverlap > 0.0f)
			{
				// Collision...
				if (xOverlap < yOverlap)
				{
					if (extent.x < 0.0f)
						c->normal = sf::Vector2f{ -1.0f, 0.0f };
					else
						c->normal = sf::Vector2f{ 1.0f, 0.0f };
					c->penetration = abs(xOverlap);
				}
				else
				{
					if (extent.y < 0.0f)
						c->normal = sf::Vector2f{ 0.0f, -1.0f };
					else
						c->normal = sf::Vector2f{ 0.0f, 1.0f };
					c->penetration = abs(yOverlap);
				}
				return true;
			}
		}
		return false;
	}
}