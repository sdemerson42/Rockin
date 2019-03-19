#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <vector>
#include <set>

namespace Core
{
	class PhysicsComponent;

	class Physics : public sde::ISystem
	{
	public:
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
		std::vector<PhysicsComponent *> m_comp;
		std::vector<PhysicsComponent *> m_staticComp;
		std::vector<Collision> m_messageGroup;
		void applyMomentum();
		void resolveCollisions();
		void adjustMomentum(Collision *c);
		void applyCorrection(Collision *c);
		bool detectCollision(Collision *c);
		void processStatics();
		void sendCollisionEvents();
	};
}
