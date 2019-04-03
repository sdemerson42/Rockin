#pragma once

#include "CoreComponentBase.h"
#include <string>
#include "SFML\Graphics.hpp"
#include <string>
#include "AutoListScene.h"

namespace Core
{
	class RenderComponent : public CoreComponentBase, public sde::AutoList<RenderComponent>, public AutoListScene<RenderComponent>
	{
		friend class Renderer;
		friend class AnimationComponent;
	public:
		RenderComponent(CoreEntity *parent, float tx, float ty, float w, float h, const std::string &fName, const std::string &layer) :
			CoreComponentBase{ parent }, m_tPos{ tx, ty }, m_tSize{ w, h }, m_texFile{ fName }, m_layer{ layer }
		{}
		const sf::Vector2f &tSize()
		{
			return m_tSize;
		}
		void setLayer(const std::string &layer)
		{
			m_layer = layer;
		}
		void setFrame(float x, float y, float w, float h)
		{
			m_tPos = { x,y };
			m_tSize = { w, h };
		}
	private:
		sf::Vector2f m_tPos;
		sf::Vector2f m_tSize;
		std::string m_texFile;
		std::string m_layer;
	};
}
