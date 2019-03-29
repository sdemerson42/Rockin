#pragma once

#include <vector>
#include "SFML\Graphics.hpp"

namespace Core
{
	struct TilesetData
	{
		std::string name;
		std::string texture;
		sf::Vector2i textureSize;
		sf::Vector2i tileSize;
		std::vector<int> staticTile;
	};
}
