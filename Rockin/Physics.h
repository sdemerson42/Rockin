#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <vector>
#include "Events.h"

namespace Core
{
	class PhysicsComponent;
	class Physics : public sde::ISystem
	{
	public:
		Physics();
		void execute() override;
	private:
		struct Collision
		{
			Collision()
			{}
			Collision(PhysicsComponent *a, PhysicsComponent *b) :
				a{ a }, b{ b }
			{}
			PhysicsComponent *a;
			PhysicsComponent *b;
			sf::Vector2f normal;
			float penetration;
		};
		struct CollisionPair
		{
			CollisionPair(PhysicsComponent *_a, PhysicsComponent *_b) :
				a{ _a }, b{ _b }
			{}
			PhysicsComponent *a;
			PhysicsComponent *b;
			bool operator==(const CollisionPair &other)
			{
				return (a == other.a && b == other.b) || (a == other.b && b == other.a);
			}
		};
		std::vector<CollisionPair> m_collisionPair;
		std::vector<std::vector<std::vector<PhysicsComponent *>>> m_pcStaticMap;
		std::vector<std::vector<std::vector<PhysicsComponent *>>> m_pcNonstaticMap;
		std::vector<Collision> m_messageGroup;
		void applyMomentum();
		void resolveCollisions(std::vector<PhysicsComponent *> &v);
		void adjustMomentum(Collision *c);
		void applyCorrection(Collision *c);
		bool detectCollision(Collision *c);
		void processStatics(std::vector<PhysicsComponent *> &sv, std::vector<PhysicsComponent *> &nsv);
		void sendCollisionEvents();
		void proxPlace(PhysicsComponent *pc, std::vector<std::vector<std::vector<PhysicsComponent *>>> &v);
		sf::Vector2i m_cellSize;

		void onNewScene(const NewSceneEvent *event);
	};
}
