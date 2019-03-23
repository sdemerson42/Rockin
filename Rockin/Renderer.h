#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <map>
#include <vector>
#include "Events.h"

namespace Core
{
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
		struct LayerData
		{
			bool isStatic;
			std::map<sf::Texture *, sf::VertexArray> vaMap;
		};
		std::map<std::string, LayerData> m_layerMap;
		std::vector<std::string> m_layerOrder;

		void onNewScene(const NewSceneEvent *event);
		void onSetCenter(const SetViewCenterEvent *event);
	};
}
