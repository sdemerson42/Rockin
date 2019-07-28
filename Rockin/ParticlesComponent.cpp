#include "ParticlesComponent.h"

namespace Core
{
	ParticlesComponent::ParticlesComponent(CoreEntity *parent, float offsetX, float offsetY,
		int direction, int arcWidth, int sizeMin, int sizeMax,
		int lifeMin, int lifeMax, float speedMin, float speedMax, int spawnMin, int spawnMax, int densityCap,
		int r, int g, int b, int a, const std::string &layer) :
		CoreComponentBase{ parent }, m_offset{ offsetX, offsetY },
		m_direction{direction}, m_arcWidth{arcWidth},
		m_sizeMin{sizeMin}, m_sizeMax{sizeMax}, m_lifeMin{lifeMin}, m_lifeMax{lifeMax},
		m_speedMin{speedMin}, m_speedMax{speedMax}, m_spawnMin{spawnMin}, m_spawnMax{spawnMax},
		m_densityCap{densityCap}, m_color{r, g, b, a}, m_particleData{ densityCap }, m_particleCount{ 0 },
		m_layer{ layer }
	{
		resetSpawnCount();
	}

	void ParticlesComponent::initialize()
	{
		resetSpawnCount();
		m_particleData.clear();
		m_particleData.resize(m_densityCap);
		m_particleCount = 0;
	}

	bool ParticlesComponent::incSpawnCounter()
	{
		if (++m_spawnCounter == m_currentSpawnTime)
		{
			resetSpawnCount();
			return true;
		}
		return false;
	}

	void ParticlesComponent::modCount(int val)
	{
		m_particleCount += val;
	}

	void ParticlesComponent::resetSpawnCount()
	{
		m_currentSpawnTime = rand() % (m_spawnMax - m_spawnMin + 1) + m_spawnMin;
		m_spawnCounter = 0;
	}
}