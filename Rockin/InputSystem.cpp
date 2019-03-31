#include "InputSystem.h"
#include "Events.h"
#include "ScriptComponent.h"
#include "SFML/System.hpp"

namespace Core
{
	void InputSystem::execute()
	{
		InputEvent ie;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			ie.stickX = -1.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			ie.stickX = 1.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			ie.stickY = -1.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			ie.stickY = 1.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			ie.fire = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			ie.esc = true;

		ScriptComponent::setInput(&ie);
	}
}