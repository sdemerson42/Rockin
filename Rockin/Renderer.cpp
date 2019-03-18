#include "Renderer.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

namespace Core
{
	Renderer::Renderer(sf::RenderWindow *window, const std::string &texF) :
		m_window{ window }
	{
		m_texture.loadFromFile(texF);
		m_states.texture = &m_texture;
	}

	void Renderer::execute()
	{
		sf::VertexArray va;
		va.setPrimitiveType(sf::PrimitiveType::Quads);
		auto sz = sde::AutoList<RenderComponent>::size();

		for (size_t i = 0; i < sz; ++i)
		{
			auto rc = sde::AutoList<RenderComponent>::get(i);
			if (!rc->active()) continue;

			auto tc = rc->parent()->getComponent<TransformComponent>();
			if (tc == nullptr) continue;

			float x = tc->position().x;
			float y = tc->position().y;
			float tx = rc->m_tPos.x;
			float ty = rc->m_tPos.y;
			float w = rc->m_tSize.x;
			float h = rc->m_tSize.y;

			va.append(sf::Vertex{ sf::Vector2f{x, y}, sf::Vector2f{tx, ty} });
			va.append(sf::Vertex{ sf::Vector2f{ x + w, y }, sf::Vector2f{ tx + w, ty } });
			va.append(sf::Vertex{ sf::Vector2f{ x + w, y + h }, sf::Vector2f{ tx + w, ty + h } });
			va.append(sf::Vertex{ sf::Vector2f{ x, y + h }, sf::Vector2f{ tx, ty + h } });
		}

		m_window->clear(sf::Color::Black);
		m_window->draw(va, m_states);
		m_window->display();
	}
}
