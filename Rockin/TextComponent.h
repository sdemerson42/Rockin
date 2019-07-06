#pragma once

#include "CoreComponentBase.h"
#include <string>
#include "SFML\Graphics.hpp"
#include "AutoListScene.h"
#include <map>

namespace Core
{
	class CoreEntity;

	/*
	TextComponent - Component containing all state necessary to describe text to be rendered
	by the Renderer, including a static map of all fonts to be used throughout execution.
	*/

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
		void setFillColor(int r, int g, int b, int a)
		{
			sf::Color color{ (sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b, (sf::Uint8)a };
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