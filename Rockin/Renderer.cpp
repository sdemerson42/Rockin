#include "Renderer.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CoreEntity.h"
#include <iostream>
#include "TextComponent.h"

namespace Core
{
	Renderer::Renderer(sf::RenderWindow *window, float vx, float vy, float vw, float vh) :
		m_window{ window }, m_view{ sf::FloatRect{vx, vy, vw, vh} }
	{
		registerFunc(this, &Renderer::onNewScene);
		registerFunc(this, &Renderer::onSetCenter);
	}

	void Renderer::execute()
	{
		// View dimensions

		float viewL = m_view.getCenter().x - m_view.getSize().x / 2.0f;
		float viewR = m_view.getCenter().x + m_view.getSize().x / 2.0f;
		float viewT = m_view.getCenter().y - m_view.getSize().y / 2.0f;
		float viewB = m_view.getCenter().y + m_view.getSize().y / 2.0f;

		auto sz = sde::AutoList<RenderComponent>::size();
		for (size_t i = 0; i < sz; ++i)
		{
			auto rc = sde::AutoList<RenderComponent>::get(i);
			if (!rc->active()) continue;
			auto tc = rc->parent()->getComponent<TransformComponent>();

			// Load texture if necessary

			if (m_texureMap.find(rc->m_texFile) == std::end(m_texureMap))
			{
				m_texureMap[rc->m_texFile].loadFromFile("Assets/Textures/" + rc->m_texFile + ".png");
			}

			// Exclude from the vertex array if out of view

			if (!m_layerMap[rc->m_layer].isStatic)
			{
				float entL = tc->position().x;
				float entR = entL + rc->m_tSize.x;
				float entT = tc->position().y;
				float entB = entT + rc->m_tSize.y;

				if (entR < viewL || entL > viewR ||
					entB < viewT || entT > viewB) continue;
			}

			// Fill vertex array in proper layer / texture

			sf::VertexArray &va = m_layerMap[rc->m_layer].vaMap[&m_texureMap[rc->m_texFile]];
			va.setPrimitiveType(sf::PrimitiveType::Quads);

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

		// Add TextComponents

		sz = sde::AutoList<TextComponent>::size();
		for (int i = 0; i < sz; ++i)
		{
			auto txc = sde::AutoList<TextComponent>::get(i);
			if (!txc->active()) continue;
			const auto &txcParentPos = txc->parent()->position();
			txc->m_text.setPosition(txcParentPos.x + txc->m_offset.x, txcParentPos.y + txc->m_offset.y);
			m_layerMap[txc->m_layer].text.push_back(&txc->m_text);
		}

		m_window->clear();
		
		// Draw layers in proper order
		
		for (const auto &layerName : m_layerOrder)
		{
			auto &ld = m_layerMap[layerName];

			// Draw vertices

			for (auto &pr : ld.vaMap)
			{
				m_states.texture = pr.first;
				
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else m_window->setView(m_view);
				
				m_window->draw(pr.second, m_states);
				pr.second.clear();
			}

			// Draw text

			for (auto txp : ld.text)
			{
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else m_window->setView(m_view);
				m_window->draw(*txp);
			}
			ld.text.clear();
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

	void Renderer::onSetCenter(const SetViewCenterEvent *event)
	{
		m_view.setCenter(event->x, event->y);
	}
}
