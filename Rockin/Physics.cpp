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
		m_comp.clear();
		m_staticComp.clear();
		m_messageGroup.clear();

		applyMomentum();
		processStatics();
		resolveCollisions();
		sendCollisionEvents();
	}

	void Physics::applyMomentum()
	{
		auto sz = sde::AutoList<PhysicsComponent>::size();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = sde::AutoList<PhysicsComponent>::get(i);
			if (!pc->active()) continue;
			if (pc->m_static)
			{
				m_staticComp.push_back(pc);
				continue;
			}
			auto tc = pc->parent()->getComponent<TransformComponent>();
			if (!tc) continue;
			float x = tc->position().x + pc->m_momentum.x;
			float y = tc->position().y + pc->m_momentum.y;
			tc->setPosition(x, y);
			m_comp.push_back(pc);
		}
	}

	void Physics::resolveCollisions()
	{
		for (size_t i = 0; i < m_comp.size(); ++i)
		{
			auto a = m_comp[i];
			if (!a->active()) continue;
			for (size_t k = i + 1; k < m_comp.size(); ++k)
			{
				auto b = m_comp[k];
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
						applyCorrection(&collision);
					}
					m_messageGroup.push_back(collision);
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

	void Physics::processStatics()
	{
		// For collisions involving static objects,
		// process axis by axis and adjust position accordingly.

		// X

		for (int i = 0; i < m_comp.size(); ++i)
		{
			auto a = m_comp[i];
			auto atc = a->parent()->getComponent<TransformComponent>();
			if (!atc) continue;
			for (int j = 0; j < m_staticComp.size(); ++j)
			{
				auto b = m_staticComp[j];
				auto btc = b->parent()->getComponent<TransformComponent>();
				if (!btc) continue;

				float e = (a->m_elasticity + b->m_elasticity) / 2.0f;

				sf::Vector2f aBoxPos{ atc->position().x + a->m_AABBOffset.x, atc->position().y + a->m_AABBOffset.y - a->m_momentum.y };
				sf::Vector2f bBoxPos{ btc->position().x + b->m_AABBOffset.x, btc->position().y + b->m_AABBOffset.y };
				sf::Vector2f aSize{ a->m_AABBSize };
				sf::Vector2f bSize{ b->m_AABBSize };

				if (aBoxPos.x + aSize.x > bBoxPos.x && aBoxPos.x < bBoxPos.x + bSize.x &&
					aBoxPos.y + aSize.y > bBoxPos.y && aBoxPos.y < bBoxPos.y + bSize.y)
				{
					if (a->m_momentum.x > 0.0f)
					{
						float diff = bBoxPos.x - (aBoxPos.x + aSize.x);
						float dist = (a->m_momentum.x - diff) * e;
						atc->setPosition(bBoxPos.x - aSize.x - a->m_AABBOffset.x - dist - 0.1f, atc->position().y);
					}
					else
					{
						float diff = aBoxPos.x - (bBoxPos.x + bSize.x);
						float dist = (a->m_momentum.x + diff) * e;
						atc->setPosition(bBoxPos.x + bSize.x - a->m_AABBOffset.x - dist + 0.1f, atc->position().y);
					}
					a->m_momentum.x *= -1.0f * e;
					m_messageGroup.push_back(Collision{ a,b });
				}
			
			}
		}

		// Y

		for (int i = 0; i < m_comp.size(); ++i)
		{
			auto a = m_comp[i];
			auto atc = a->parent()->getComponent<TransformComponent>();
			if (!atc) continue;
			for (int j = 0; j < m_staticComp.size(); ++j)
			{
				auto b = m_staticComp[j];
				auto btc = b->parent()->getComponent<TransformComponent>();
				if (!btc) continue;

				float e = (a->m_elasticity + b->m_elasticity) / 2.0f;

				sf::Vector2f aBoxPos{ atc->position().x + a->m_AABBOffset.x, atc->position().y + a->m_AABBOffset.y };
				sf::Vector2f bBoxPos{ btc->position().x + b->m_AABBOffset.x, btc->position().y + b->m_AABBOffset.y };
				sf::Vector2f aSize{ a->m_AABBSize };
				sf::Vector2f bSize{ b->m_AABBSize };

				if (aBoxPos.x + aSize.x > bBoxPos.x && aBoxPos.x < bBoxPos.x + bSize.x &&
					aBoxPos.y + aSize.y > bBoxPos.y && aBoxPos.y < bBoxPos.y + bSize.y)
				{
					if (a->m_momentum.y > 0.0f)
					{
						float diff = bBoxPos.y - (aBoxPos.y + aSize.y);
						float dist = (a->m_momentum.y - diff) * e;
						atc->setPosition(atc->position().x, bBoxPos.y - aSize.y - a->m_AABBOffset.y - dist - 0.1f );
					}
					else
					{
						float diff = aBoxPos.y - (bBoxPos.y + bSize.y);
						float dist = (a->m_momentum.y + diff) * e;
						atc->setPosition(atc->position().x, bBoxPos.y + bSize.y - a->m_AABBOffset.y - dist + 0.1f);
					}
					a->m_momentum.y *= -1.0f * e;
					if (find_if(std::begin(m_messageGroup), std::end(m_messageGroup), [=](const Collision &col)
					{
						return col.a == a && col.b == b;
					}) == std::end(m_messageGroup)) m_messageGroup.push_back(Collision{ a,b });
				}
			}
		}
	}

	void Physics::sendCollisionEvents()
	{
		// Alert scripts if applicable

		for (const auto &c : m_messageGroup)
		{

			auto ae = c.a->parent();
			auto asc = ae->getComponent<ScriptComponent>();
			auto be = c.b->parent();
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