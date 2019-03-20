#pragma once

#include "CoreComponentBase.h"
#include <string>
#include "SFML\Graphics.hpp"

namespace Core
{
	class RenderComponent : public CoreComponentBase, public sde::AutoList<RenderComponent>
	{
		friend class Renderer;
		friend class AnimationComponent;
	public:
		RenderComponent(CoreEntity *parent, float tx, float ty, float w, float h) :
			CoreComponentBase{ parent }, m_tPos{ tx, ty }, m_tSize{ w, h }
		{}
	private:
		sf::Vector2f m_tPos;
		sf::Vector2f m_tSize;
	};
}
