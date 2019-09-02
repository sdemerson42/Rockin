#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <map>
#include <vector>
#include "Events.h"

namespace Core
{
	class Sim;

	/*
	Renderer - Stores textures and processes state from TextComponents and RenderComponents
	to draw the current scene. When processing components the Renderer sorts by layers and
	packs quads into as few sf::VertexArrays as possible to minimize draw calls.
	*/

	class Renderer : public sde::ISystem
	{
	public:
		Renderer(Sim *sim, sf::RenderWindow *window, float vx, float vy, float vw, float vh);
		void execute() override;
		void buildTilemapTexture(float vl, float vr, float vt, float vb, const std::vector<int> &tiles, bool force = false);
	private:
		Sim *m_sim;
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
		sf::Vector2i m_tilemapSize;
		sf::Vector2i m_tileSize;
		std::vector<int> *m_tilemap;
		int m_tilemapBoundsTile;
		const int m_tilemapCellSize = 3200;
		sf::Vector2i m_tilesetTextureSize;
		bool m_tilemapEditFlag;

		float m_viewL;
		float m_viewR;
		float m_viewT;
		float m_viewB;

		void onNewScene(const NewSceneEvent *event);
		void onSetCenter(const SetViewCenterEvent *event);
		void onTilemapEdit(const TilemapEditEvent *event);
	};
}
