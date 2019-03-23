#include "Renderer.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CoreEntity.h"
#include <iostream>

namespace Core
{
	Renderer::Renderer(sf::RenderWindow *window, const std::string &texF) :
		m_window{ window }
	{
		registerFunc(this, &Renderer::onNewScene);
	}

	void Renderer::execute()
	{
		auto sz = sde::AutoList<RenderComponent>::size();
		for (size_t i = 0; i < sz; ++i)
		{
			auto rc = sde::AutoList<RenderComponent>::get(i);
			if (!rc->active()) continue;
			auto tc = rc->parent()->getComponent<TransformComponent>();

			// Load texture if necessary

			if (m_texureMap.find(rc->m_texFile) == std::end(m_texureMap))
			{
				m_texureMap[rc->m_texFile].loadFromFile("Assets/" + rc->m_texFile + ".png");
				m_layerMap[rc->m_layer].vaMap[&m_texureMap[rc->m_texFile]].setPrimitiveType(sf::Quads);
			}

			// Fill vertex array in proper layer / texture

			sf::VertexArray &va = m_layerMap[rc->m_layer].vaMap[&m_texureMap[rc->m_texFile]];

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
		
		// Draw layers in proper order

		for (const auto &layerName : m_layerOrder)
		{
			auto &ld = m_layerMap[layerName];
			for (auto &pr : ld.vaMap)
			{
				m_states.texture = pr.first;
				m_window->draw(pr.second, m_states);
				pr.second.clear();
			}
		}
	
		m_window->display();
	}

	void Renderer::onNewScene(const NewSceneEvent *event)
	{
		m_layerMap.clear();
		m_layerOrder.clear();
		for (int i = 0; i < event->isStatic.size(); ++i)
		{
			m_layerOrder.push_back(event->layer[i]);
			m_layerMap[event->layer[i]].isStatic = event->isStatic[i];
		}
	}
}
