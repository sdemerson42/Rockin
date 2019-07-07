#include "Physics.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "ScriptComponent.h"
#include "CoreEntity.h"
#include <algorithm>
#include <iostream>

namespace Core
{
	Physics::Physics()
	{
		registerFunc(this, &Physics::onNewScene);
	}

	void Physics::execute()
	{
		m_collisionPair.clear();
		for (auto &vv : m_pcNonstaticMap)
		{
			for (auto &v : vv)
			{
				v.clear();
			}
		}

		auto sz = AutoListScene<PhysicsComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = AutoListScene<PhysicsComponent>::alsCurrentGet(i);
			if (!pc->getStatic() && pc->active() && !pc->noCollision()) proxPlace(pc, m_pcNonstaticMap);
		}
		
		m_messageGroup.clear();

		applyMomentum();

		for (int i = 0; i < m_pcStaticMap.size(); ++i)
		{
			for (int j = 0; j < m_pcStaticMap[0].size(); ++j)
			{
				processStatics(m_pcStaticMap[i][j], m_pcNonstaticMap[i][j]);
			}
		}

		for (int i = 0; i < m_pcNonstaticMap.size(); ++i)
		{
			for (int j = 0; j < m_pcNonstaticMap[0].size(); ++j)
			{
				resolveCollisions(m_pcNonstaticMap[i][j]);
			}
		}
	
		sendCollisionEvents();
	}

	void Physics::applyMomentum()
	{
		auto sz = AutoListScene<PhysicsComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = AutoListScene<PhysicsComponent>::alsCurrentGet(i);
			if (pc->active() && !pc->getStatic())
			{
				auto e = pc->parent();
				float x = e->position().x + pc->momentum().x;
				float y = e->position().y + pc->momentum().y;
				e->setPosition(x, y);
			}
		}
	}

	void Physics::resolveCollisions(std::vector<PhysicsComponent *> &v)
	{
		for (size_t i = 0; i < v.size(); ++i)
		{
			auto a = v[i];
			for (size_t k = i + 1; k <v.size(); ++k)
			{
				auto b = v[k];
				// Escape conditions

				CollisionPair cp{ a,b };
				if (std::find(std::begin(m_collisionPair), std::end(m_collisionPair), cp) != std::end(m_collisionPair)) continue;
				m_collisionPair.push_back(cp);

				if (0.0f == a->iMass() && 0.0f == b->iMass()) continue;
			
				Collision collision{ a, b };
				if (detectCollision(&collision))
				{
					if (a->solid() && b->solid())
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
		// Resolve collisions using: v'a = va - jn / massA, v'b = vb + jn / massB

		auto a = collision->a;
		auto b = collision->b;

		float e = (a->elasticity() + b->elasticity()) / 2.0f;
		auto relativeVelocity = sf::Vector2f{ b->momentum().x - a->momentum().x, b->momentum().y - a->momentum().y };
		float velAlongNormal = relativeVelocity.x * collision->normal.x + 
			relativeVelocity.y * collision->normal.y;
		if (velAlongNormal > 0.0f) return;

		// Find impule magnitude by j = (-(e+1)(vb - va) . n) / sum of inverse masses 

		float j = (-1.0f*(e + 1.0f) * velAlongNormal) / (a->iMass() + b->iMass());

		a->setMomentum(a->momentum() - j * collision->normal * a->iMass());
		b->setMomentum(b->momentum() + j * collision->normal * b->iMass());
	}

	void Physics::applyCorrection(Collision *collision)
	{
		const float slop = 0.1f;
		const float percCorrect = 0.8f;

		auto ta = collision->a->parent()->getComponent<TransformComponent>();
		auto tb = collision->b->parent()->getComponent<TransformComponent>();

		sf::Vector2f correction = std::max(collision->penetration - slop, 0.0f) / 
			(collision->a->iMass() + collision->b->iMass()) * percCorrect * collision->normal;
		ta->adjustPosition(-1.0f * collision->a->iMass() * correction);
		tb->adjustPosition(collision->b->iMass() * correction);
	}

	bool Physics::detectCollision(Collision *c)
	{
		auto atc = c->a->parent()->getComponent<TransformComponent>();
		if (!atc) return false;
		auto btc = c->b->parent()->getComponent<TransformComponent>();
		if (!btc) return false;

		sf::Vector2f aBoxPos{ atc->position().x + c->a->boundingBoxOffset().x, 
			atc->position().y + c->a->boundingBoxOffset().y };
		sf::Vector2f bBoxPos{ btc->position().x + c->b->boundingBoxOffset().x, 
			btc->position().y + c->b->boundingBoxOffset().y };
		sf::Vector2f aSize{ c->a->boundingBoxSize() };
		sf::Vector2f bSize{ c->b->boundingBoxSize() };
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

	void Physics::processStatics(std::vector<PhysicsComponent *> &sv, std::vector<PhysicsComponent *> &nsv)
	{
		// For collisions involving static objects,
		// process axis by axis and adjust position accordingly.

		// X axis

		for (int i = 0; i < nsv.size(); ++i)
		{
			auto a = nsv[i];
			auto atc = a->parent()->getComponent<TransformComponent>();
			for (int j = 0; j < sv.size(); ++j)
			{
				auto b = sv[j];
				auto btc = b->parent()->getComponent<TransformComponent>();
				
				CollisionPair cp{ a,b };
				if (std::find(std::begin(m_collisionPair), std::end(m_collisionPair), cp) != std::end(m_collisionPair)) continue;
				m_collisionPair.push_back(cp);
				
				float e = (a->elasticity() + b->elasticity()) / 2.0f;

				sf::Vector2f aBoxPos{ atc->position().x + a->boundingBoxOffset().x, 
					atc->position().y + a->boundingBoxOffset().y - a->momentum().y };
				sf::Vector2f bBoxPos{ btc->position().x + b->boundingBoxOffset().x, 
					btc->position().y + b->boundingBoxOffset().y };
				sf::Vector2f aSize{ a->boundingBoxSize() };
				sf::Vector2f bSize{ b->boundingBoxSize() };

				if (aBoxPos.x + aSize.x > bBoxPos.x && aBoxPos.x < bBoxPos.x + bSize.x &&
					aBoxPos.y + aSize.y > bBoxPos.y && aBoxPos.y < bBoxPos.y + bSize.y)
				{
					if (a->solid())
					{
						if (a->momentum().x > 0.0f)
						{
							float diff = bBoxPos.x - (aBoxPos.x + aSize.x);
							float dist = (a->momentum().x - diff) * e;
							atc->setPosition(bBoxPos.x - aSize.x - a->boundingBoxOffset().x - dist - 0.1f, atc->position().y);
						}
						else
						{
							float diff = aBoxPos.x - (bBoxPos.x + bSize.x);
							float dist = (a->momentum().x + diff) * e;
							atc->setPosition(bBoxPos.x + bSize.x - a->boundingBoxOffset().x - dist + 0.1f, atc->position().y);
						}
						auto aMomentum = a->momentum();
						a->setMomentum(aMomentum.x * -1.0f * e, aMomentum.y);
					}
					m_messageGroup.push_back(Collision{ a,b });
				}
			
			}
		}

		// Y axis

		for (int i = 0; i < nsv.size(); ++i)
		{
			auto a = nsv[i];
			auto atc = a->parent()->getComponent<TransformComponent>();
			
			for (int j = 0; j < sv.size(); ++j)
			{
				auto b = sv[j];
				auto btc = b->parent()->getComponent<TransformComponent>();
				if (!btc) continue;

				float e = (a->elasticity() + b->elasticity()) / 2.0f;

				sf::Vector2f aBoxPos{ atc->position().x + a->boundingBoxOffset().x, 
					atc->position().y + a->boundingBoxOffset().y };
				sf::Vector2f bBoxPos{ btc->position().x + b->boundingBoxOffset().x, 
					btc->position().y + b->boundingBoxOffset().y };
				sf::Vector2f aSize{ a->boundingBoxSize() };
				sf::Vector2f bSize{ b->boundingBoxSize() };

				if (aBoxPos.x + aSize.x > bBoxPos.x && aBoxPos.x < bBoxPos.x + bSize.x &&
					aBoxPos.y + aSize.y > bBoxPos.y && aBoxPos.y < bBoxPos.y + bSize.y)
				{
					if (a->solid())
					{
						if (a->momentum().y > 0.0f)
						{
							float diff = bBoxPos.y - (aBoxPos.y + aSize.y);
							float dist = (a->momentum().y - diff) * e;
							atc->setPosition(atc->position().x, bBoxPos.y - aSize.y - a->boundingBoxOffset().y - dist - 0.1f);
						}
						else
						{
							float diff = aBoxPos.y - (bBoxPos.y + bSize.y);
							float dist = (a->momentum().y + diff) * e;
							atc->setPosition(atc->position().x, bBoxPos.y + bSize.y - a->boundingBoxOffset().y - dist + 0.1f);
						}
						auto aMomentum = a->momentum();
						a->setMomentum(aMomentum.x, aMomentum.y * -1.0f * e);
					}

					// Check to see if this collision has already been recorded in
					// the message group before adding it.

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
				ce.colliderPhysics = c.b;
				broadcast(&ce);
			}
			if (bsc)
			{
				CollisionEvent ce;
				ce.collider = ae;
				ce.receiver = bsc;
				ce.colliderPhysics = c.a;
				broadcast(&ce);
			}
		}
	}

	void Physics::proxPlace(PhysicsComponent *pc, std::vector<std::vector<std::vector<PhysicsComponent *>>> &proxMap)
	{
		// Determine where in the provided 2D vector (proximity map) the
		// provided PhysicsComponent should be placed.

		int l = (int)pc->parent()->position().x + (int)pc->boundingBoxOffset().x;
		int t = (int)pc->parent()->position().y + (int)pc->boundingBoxOffset().y;
		int r = l + (int)pc->boundingBoxSize().x + (int)pc->momentum().x + 1;
		int b = t + (int)pc->boundingBoxSize().y + (int)pc->momentum().y + 1;

		int xSz = proxMap.size();
		int ySz = proxMap[0].size();

		int xMin = std::max(0, l / m_cellSize.x);
		int xMax = std::min(r / m_cellSize.y, xSz - 1);
		int yMin = std::max(0, t / m_cellSize.y);
		int yMax = std::min(b / m_cellSize.y, ySz - 1);

		for (int i = xMin; i <= xMax; ++i)
		{
			for (int j = yMin; j <= yMax; ++j)
			{
				proxMap[i][j].push_back(pc);
			}
		}
	}

	void Physics::onNewScene(const NewSceneEvent *event)
	{
		// Set state that determines behavior of proximity map

		m_cellSize = event->cellSize;
		m_pcStaticMap.clear();
		m_pcNonstaticMap.clear();

		int xSize = event->sceneSize.x / event->cellSize.x +
			(event->sceneSize.x % event->cellSize.x == 0 ? 0 : 1);
		int ySize = event->sceneSize.y / event->cellSize.y +
			(event->sceneSize.y % event->cellSize.y == 0 ? 0 : 1);

		m_pcNonstaticMap.resize(xSize);
		for (auto &vv : m_pcNonstaticMap) vv.resize(ySize);

		m_pcStaticMap.resize(xSize);
		for (auto &vv : m_pcStaticMap) vv.resize(ySize);

		// Add statics to map

		auto sz = AutoListScene<PhysicsComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = AutoListScene<PhysicsComponent>::alsCurrentGet(i);
			if (pc->getStatic()) proxPlace(pc, m_pcStaticMap);
		}
	}
}