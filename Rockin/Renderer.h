#pragma once

#include "sde.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <map>
#include "Events.h"

namespace Core
{
	class Renderer : public sde::ISystem
	{
	public:
		Renderer(sf::RenderWindow *window, const std::string &texF);
		void execute() override;
	private:
		sf::RenderWindow *m_window;
		sf::Texture m_texture;
		sf::RenderStates m_states;

		void onNewScene(const NewSceneEvent *event);
	};
}
