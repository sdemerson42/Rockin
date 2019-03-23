#include "TextComponent.h"

namespace Core
{
	std::map<std::string, sf::Font> TextComponent::m_sFont;

	TextComponent::TextComponent(CoreEntity *parent, const std::string &s, const std::string &fontFile, 
		const std::string &layer, float offX, float offY, int size, int r, int g, int b, int a) :
		CoreComponentBase{ parent }, m_layer{ layer }, m_offset{ offX, offY }
	{
		if (m_sFont.find(fontFile) == std::end(m_sFont))
		{
			m_sFont[fontFile].loadFromFile("Assets/Fonts/" + fontFile + ".ttf");
		}
		m_text.setString(s);
		m_text.setFont(m_sFont[fontFile]);
		m_text.setCharacterSize(size);
		sf::Color color;
		color.r = (sf::Uint8) r;
		color.g = (sf::Uint8) g;
		color.b = (sf::Uint8) b;
		color.a = (sf::Uint8) a;
		m_text.setFillColor(color);
	}
}