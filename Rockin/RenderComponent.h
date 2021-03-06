#pragma once

#include "CoreComponentBase.h"
#include <string>
#include "SFML\Graphics.hpp"
#include <string>
#include "AutoListScene.h"
#include "TransformComponent.h"
#include "CoreEntity.h"

namespace Core
{
	/*
	RenderComponent - Contains state necessary for Entities to be rendered and methods
	to simplify interactions with scripts and AnimationComponents.
	Textures are owned by the Renderer class to avoid redundant state.
	*/

	class RenderComponent : public CoreComponentBase, public sde::AutoList<RenderComponent>, public AutoListScene<RenderComponent>
	{
	public:
		RenderComponent(CoreEntity *parent, float tx, float ty, float w, float h, const std::string &fName, const std::string &layer) :
			CoreComponentBase{ parent }, m_tPos{ tx, ty }, m_tSize{ w, h }, m_texFile{ fName }, m_layer{ layer }, m_tFlag{ false },
			m_cFlag{ false }, m_rotation{ 0.0f }, m_scale{ 1.0f, 1.0f }
		{}
		// Accessors
		const sf::Vector2f &textureSize() const
		{
			return m_tSize;
		}
		void setTextureSize(float x, float y)
		{
			m_tSize.x = x;
			m_tSize.y = y;
		}
		void setTextureSize(const sf::Vector2f &vec)
		{
			m_tSize = vec;
		}
		const sf::Vector2f &texturePosition() const
		{
			return m_tPos;
		}
		void setTexturePosition(float x, float y)
		{
			m_tPos.x = x;
			m_tPos.y = y;
		}
		void setTexturePosition(const sf::Vector2f &vec)
		{
			m_tPos = vec;
		}
		void setLayer(const std::string &layer)
		{
			m_layer = layer;
		}
		void setFrame(float x, float y, float w, float h)
		{
			m_tPos = { x,y };
			m_tSize = { w, h };
		}
		void setScale(float x, float y)
		{
			m_scale = { x, y };
			if (m_scale.x == 1.0f && m_scale.y == 1.0f && m_rotation == 0.0f) m_tFlag = false;
			else m_tFlag = true;
		}
		void setColor(int r, int g, int b, int a)
		{
			m_color.r = (sf::Uint8)r;
			m_color.g = (sf::Uint8)g;
			m_color.b = (sf::Uint8)b;
			m_color.a = (sf::Uint8)a;
			if (m_color == sf::Color::White) m_cFlag = false;
			m_cFlag = true;
		}
		void setRotation(float degrees)
		{
			m_rotation = degrees;
			if (m_scale.x == 1.0f && m_scale.y == 1.0f && m_rotation == 0.0f) m_tFlag = false;
			else m_tFlag = true;
		}
		const std::string &textureFile() const
		{
			return m_texFile;
		}
		const std::string&layer() const
		{
			return m_layer;
		}
		bool tFlag() const
		{
			return m_tFlag;
		}
		bool cFlag() const
		{
			return m_cFlag;
		}
		const sf::Vector2f &scale() const
		{
			return m_scale;
		}
		float rotation() const
		{
			return m_rotation;
		}
		const sf::Color &color() const
		{
			return m_color;
		}
	private:
		sf::Vector2f m_tPos;
		sf::Vector2f m_tSize;
		std::string m_texFile;
		std::string m_layer;
		sf::Vector2f m_scale;
		float m_rotation;
		sf::Color m_color;

		bool m_tFlag;
		bool m_cFlag;
	};
}
