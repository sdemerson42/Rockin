#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"

namespace Core
{
	class CoreEntity : public sde::EntityNoParent, public sde::AutoList<CoreEntity>
	{
	public:
		CoreEntity();
		bool persistent() const;
		void setPersistent(bool b);
		const sf::Vector2f &position();
		void setPosition(const sf::Vector2f &v);
		void setPosition(float x, float y);
		void adjustPosition(const sf::Vector2f &v);
		void adjustPosition(float x, float y);
	private:
		bool m_persistent;
	};
}
