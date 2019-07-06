#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"

namespace Core
{
	/*
	CoreEntity - This is the main game object class. CoreEntities have no state of their own aside
	from a persistence flag. Instead they provide ownership of and access to components, which
	are stored in the sde::EntityNoParent subclass.
	*/
	class ScriptComponent;
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
		ScriptComponent *scriptComponent();
	private:
		bool m_persistent;
	};
}
