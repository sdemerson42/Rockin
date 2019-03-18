#pragma once

#include "sde.h"
#include <string>
#include "SFML\Graphics.hpp"

namespace Core
{
	class RenderComponent : public sde::ComponentBase, public sde::AutoList<RenderComponent>
	{
		friend class Renderer;
	public:
		RenderComponent(sde::Entity *parent, float tx, float ty, float w, float h) :
			ComponentBase{ parent }, m_tPos{ tx, ty }, m_tSize{ w, h }
		{}
	private:
		sf::Vector2f m_tPos;
		sf::Vector2f m_tSize;
	};
}
