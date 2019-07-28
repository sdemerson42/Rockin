#include "Particles.h"
#include "ParticlesComponent.h"
#include <math.h>
#include "TransformComponent.h"
#include "CoreEntity.h"

namespace Core
{
	const float Particles::pi{ 3.141592f };
	const float Particles::radPerDeg{ 3.141592f / 180.0f };

	void Particles::execute()
	{
		auto sz = AutoListScene<ParticlesComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = AutoListScene<ParticlesComponent>::alsCurrentGet(i);
			if (!pc->active()) continue;

			// Run counter and spawn particle if necessary

			if (pc->incSpawnCounter())
			{
				if (pc->count() < pc->densityCap())
				{
					// spawn particle

					auto &pData = pc->particleData()[pc->count()];

					auto parentPosition = pc->parent()->position();

					int speedMin = (int)(pc->speedMin() * 100.0f);
					int speedMax = (int)(pc->speedMax() * 100.0f);
					float speed{ (float)(rand() % (speedMax - speedMin + 1) + speedMin) / 100.0f };
					
					int arc = rand() % pc->arcWidth() + pc->direction();
					float radians = radPerDeg * (float)arc;
					pData.direction.x = cos(radians) * speed;
					pData.direction.y = sin(radians) * speed * -1.0f;

					auto lifeMin{ pc->lifeMin() };
					pData.life = rand() % (pc->lifeMax() - lifeMin + 1) + lifeMin;
					pData.lifeCounter = 0;
					pData.position = parentPosition + pc->offset();
					auto sizeMin{ pc->sizeMin() };
					pData.size = rand() % (pc->sizeMax() - sizeMin + 1) + sizeMin;

					pc->modCount(1);
				}
			}

			// Move current particles and despawn if necessary

			auto count = pc->count();
			for (int i = 0; i < count; ++i)
			{
				auto &pData = pc->particleData()[i];
				if (++pData.lifeCounter == pData.life)
				{
					pc->modCount(-1);
					--count;
					auto topData = pc->particleData()[count];
					pData = topData;
					continue;
				}

				pData.position += pData.direction;
			}
		}
	}
}