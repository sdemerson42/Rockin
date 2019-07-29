#pragma once

#include "CoreComponentBase.h"
#include "AutoListScene.h"
#include "SFML\Graphics.hpp"
#include <vector>
#include <string>

namespace Core
{

	/*
	ParticlesComponent - Allows basic particle effects to be added to Entities. The component
	holds state defining the overall functioning of the particle system as well as state describing
	individual particles. Particle movement, spawning, and lifetime management is handled by
	the Particles system. Particle rendering is handled by the Renderer.
	*/

	class ParticlesComponent : public CoreComponentBase, public sde::AutoList<ParticlesComponent>, public AutoListScene<ParticlesComponent>
	{
	public:
		struct ParticleData
		{
			sf::Vector2f position;
			sf::Vector2f direction;
			int size;
			int life;
			int lifeCounter;
		};
		ParticlesComponent(CoreEntity *parent, float offsetX, float offsetY,
			int direction, int arcWidth, int sizeMin, int sizeMax,
			int lifeMin, int lifeMax, float speedMin, float speedMax, int spawnMin, int spawnMax, int densityCap,
			bool despawnPersist, int r, int g, int b, int a, const std::string &layer);
		void initialize() override;
		bool incSpawnCounter();
		void modCount(int val);

		// Accessors

		const sf::Vector2f &offset()
		{
			return m_offset;
		}
		int direction() const
		{
			return m_direction;
		}
		int arcWidth() const
		{
			return m_arcWidth;
		}
		int sizeMin() const
		{
			return m_sizeMin;
		}
		int sizeMax() const
		{
			return m_sizeMax;
		}
		int lifeMin() const
		{
			return m_lifeMin;
		}
		int lifeMax() const
		{
			return m_lifeMax;
		}
		float speedMin() const
		{
			return m_speedMin;
		}
		float speedMax() const
		{
			return m_speedMax;
		}
		int spawnMin() const
		{
			return m_spawnMin;
		}
		int spawnMax() const
		{
			return m_spawnMax;
		}
		int densityCap() const
		{
			return m_densityCap;
		}
		const sf::Color &color() const
		{
			return m_color;
		}
		std::vector<ParticleData> &particleData()
		{
			return m_particleData;
		}
		int count() const
		{
			return m_particleCount;
		}
		const std::string &layer()
		{
			return m_layer;
		}
		bool despawnPersist() const
		{
			return m_despawnPersist;
		}
	private:
		sf::Vector2f m_offset;
		int m_direction;
		int m_arcWidth;
		int m_sizeMin;
		int m_sizeMax;
		int m_lifeMin;
		int m_lifeMax;
		float m_speedMin;
		float m_speedMax;
		int m_spawnMin;
		int m_spawnMax;
		int m_densityCap;
		sf::Color m_color;
		bool m_despawnPersist;
		std::string m_layer;

		int m_currentSpawnTime;
		int m_spawnCounter;
		std::vector<ParticleData> m_particleData;
		int m_particleCount;

		void resetSpawnCount();
	};
}