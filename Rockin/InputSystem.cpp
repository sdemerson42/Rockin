#include "InputSystem.h"
#include "Events.h"
#include "ScriptComponent.h"
#include "SFML/System.hpp"

namespace Core
{
	void InputSystem::execute()
	{
		if (sf::Joystick::isConnected(0))
		{
			float val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) * 0.01f;
			m_ie.stickLeftX = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) * 0.01f;
			m_ie.stickLeftY = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) * 0.01f;
			m_ie.stickRightX = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::R) * 0.01f;
			m_ie.stickRightY = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) * 0.01f;
			m_ie.padX = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) * 0.01f;
			m_ie.padY = (abs(val) < deadZone ? 0.0f : val);
			val = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) * 0.01f;
			m_ie.trigger = (abs(val) < deadZone ? 0.0f : val);


			if (sf::Joystick::isButtonPressed(0, 0))
			{
				if (m_ie.a < 3) ++m_ie.a;
			}
			else m_ie.a = 0;
			if (sf::Joystick::isButtonPressed(0, 1))
			{
				if (m_ie.b < 3) ++m_ie.b;
			}
			else m_ie.b = 0;
			if (sf::Joystick::isButtonPressed(0, 2))
			{
				if (m_ie.x < 3) ++m_ie.x;
			}
			else m_ie.x = 0;
			if (sf::Joystick::isButtonPressed(0, 3))
			{
				if (m_ie.y < 3) ++m_ie.y;
			}
			else m_ie.y = 0;
			if (sf::Joystick::isButtonPressed(0, 4))
			{
				if (m_ie.shoulderLeft < 3) ++m_ie.shoulderLeft;
			}
			else m_ie.shoulderLeft = 0;
			if (sf::Joystick::isButtonPressed(0, 5))
			{
				if (m_ie.shoulderRight < 3) ++m_ie.shoulderRight;
			}
			else m_ie.shoulderRight = 0;
			if (sf::Joystick::isButtonPressed(0, 6))
			{
				if (m_ie.select < 3) ++m_ie.select;
			}
			else m_ie.select = 0;
			if (sf::Joystick::isButtonPressed(0, 7))
			{
				if (m_ie.start < 3) ++m_ie.start;
			}
			else m_ie.start = 0;
			if (sf::Joystick::isButtonPressed(0, 8))
			{
				if (m_ie.stickLeftPush < 3) ++m_ie.stickLeftPush;
			}
			else m_ie.stickLeftPush = 0;
			if (sf::Joystick::isButtonPressed(0, 9))
			{
				if (m_ie.stickRightPush < 3) ++m_ie.stickRightPush;
			}
			else m_ie.stickRightPush = 0;
		}
		else
		{
			// Keyboard controls

			m_ie.stickLeftX = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) m_ie.stickLeftX = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) m_ie.stickLeftX = 1.0f;

			m_ie.stickLeftY = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) m_ie.stickLeftY = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) m_ie.stickLeftY = 1.0f;

			m_ie.stickRightX = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) m_ie.stickRightX = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) m_ie.stickRightX = 1.0f;

			m_ie.stickRightY = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) m_ie.stickRightY = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) m_ie.stickRightY = 1.0f;

			m_ie.padX = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) m_ie.padX = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) m_ie.padX = 1.0f;

			m_ie.padY = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) m_ie.padY = -1.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) m_ie.padY = 1.0f;

			m_ie.trigger = 0.0f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) m_ie.trigger = 1.0f;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (m_ie.a < 3) ++m_ie.a;
			}
			else m_ie.a = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				if (m_ie.b < 3) ++m_ie.b;
			}
			else m_ie.b = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
			{
				if (m_ie.x < 3) ++m_ie.x;
			}
			else m_ie.x = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
			{
				if (m_ie.y < 3) ++m_ie.y;
			}
			else m_ie.y = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				if (m_ie.shoulderLeft < 3) ++m_ie.shoulderLeft;
			}
			else m_ie.shoulderLeft = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			{
				if (m_ie.shoulderRight < 3) ++m_ie.shoulderRight;
			}
			else m_ie.shoulderRight = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
			{
				if (m_ie.select < 3) ++m_ie.select;
			}
			else m_ie.select = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{
				if (m_ie.start < 3) ++m_ie.start;
			}
			else m_ie.start = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				if (m_ie.stickLeftPush < 3) ++m_ie.stickLeftPush;
			}
			else m_ie.stickLeftPush = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			{
				if (m_ie.stickRightPush < 3) ++m_ie.stickRightPush;
			}
			else m_ie.stickRightPush = 0;
		}

		ScriptComponent::setInput(&m_ie);
	}
}