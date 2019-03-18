#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"

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
		void applyMomentum();
		void resolveCollisions();
		void adjustMomentum(Collision *c);
		void applyCorrection(Collision *c);
		bool detectCollision(Collision *c);
	};
}
