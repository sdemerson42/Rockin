#pragma once

#include "CoreComponentBase.h"
#include <string>
#include "SFML\Graphics.hpp"
#include "AutoListScene.h"
#include <map>

namespace Core
{
	class CoreEntity;
	class TextComponent : public CoreComponentBase, public sde::AutoList<TextComponent>, public AutoListScene<TextComponent>
	{
		friend class Renderer;
	public:
		TextComponent(CoreEntity *parent, const std::string &s,
			const std::string &fontFile, const std::string &layer = "default", 
			float offX = 0.0f, float offY = 0.0f, int size = 20, int r = 255, int g = 255, int b = 255, int a = 255);
		void setString(const std::string &s)
		{
			m_text.setString(s);
		}
		void setFillColor(const sf::Color &color)
		{
			m_text.setFillColor(color);
		}
		void setSize(int sz)
		{
			m_text.setCharacterSize(sz);
		}
	private:
		sf::Text m_text;
		std::string m_layer;
		sf::Vector2f m_offset;

		static std::map<std::string, sf::Font> m_sFont;
	};
}