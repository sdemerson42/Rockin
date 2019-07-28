#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <map>
#include <vector>
#include "Events.h"

namespace Core
{
	/*
	Renderer - Stores textures and processes state from TextComponents and RenderComponents
	to draw the current scene. When processing components the Renderer sorts by layers and
	packs quads into as few sf::VertexArrays as possible to minimize draw calls.
	*/

	class Renderer : public sde::ISystem
	{
	public:
		Renderer(sf::RenderWindow *window, float vx, float vy, float vw, float vh);
		void execute() override;
	private:
		sf::RenderWindow *m_window;
		sf::RenderStates m_states;
		sf::View m_view;
		std::map<std::string, sf::Texture> m_texureMap;
		struct VaData
		{
			sf::VertexArray va;
			sf::Color color;
			sf::Transform transform;
			bool modified{ false };
		};
		struct LayerData
		{
			bool isStatic;
			std::map<sf::Texture *, std::vector<VaData>> vaMap;
			std::vector<sf::Text *> text;
			std::vector<sf::VertexArray> particle;
		};
		std::map<std::string, LayerData> m_layerMap;
		std::vector<std::string> m_layerOrder;

		sf::RenderTexture m_tilemapRenderTexture;
		sf::Texture m_tilemapTexture;
		sf::Sprite m_tilemapSprite;
		std::string m_tilemapLayer;

		void onNewScene(const NewSceneEvent *event);
		void onSetCenter(const SetViewCenterEvent *event);
	};
}
