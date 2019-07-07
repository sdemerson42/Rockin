#include "Renderer.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CoreEntity.h"
#include <iostream>
#include "TextComponent.h"
#include "TilesetData.h"

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

		auto sz = AutoListScene<RenderComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto rc = AutoListScene<RenderComponent>::alsCurrentGet(i);
			if (!rc->active()) continue;
			auto tc = rc->parent()->getComponent<TransformComponent>();

			// Load texture if necessary

			auto textureFile = rc->textureFile();

			if (m_texureMap.find(textureFile) == std::end(m_texureMap))
			{
				m_texureMap[textureFile].loadFromFile(textureFile);
			}

			// Exclude from the vertex array if out of view

			auto layer = rc->layer();

			if (!m_layerMap[layer].isStatic)
			{
				float entL = tc->position().x;
				float entR = entL + rc->textureSize().x;
				float entT = tc->position().y;
				float entB = entT + rc->textureSize().y;

				if (entR < viewL || entL > viewR ||
					entB < viewT || entT > viewB) continue;
			}

			// Fill vertex array in proper layer / texture

			auto &vvd = m_layerMap[layer].vaMap[&m_texureMap[textureFile]];
			if (vvd.size() == 0) vvd.push_back(VaData{});

			if (rc->tFlag() || rc->cFlag())
			{
				vvd.push_back(VaData{});
				vvd[vvd.size() - 1].modified = true;
				sf::Vector2f center{ tc->position().x + rc->textureSize().x / 2.0f,
					tc->position().y + rc->textureSize().y / 2.0f };
				vvd[vvd.size() - 1].transform.rotate(rc->rotation(), center);
				vvd[vvd.size() - 1].transform.scale(rc->scale(), center);
				vvd[vvd.size() - 1].color = rc->color();
			}

			sf::VertexArray &va = ((rc->tFlag() || rc->cFlag()) ? vvd[vvd.size() - 1].va : vvd[0].va);

			va.setPrimitiveType(sf::PrimitiveType::Quads);

			float x = tc->position().x;
			float y = tc->position().y;
			float tx = rc->texturePosition().x;
			float ty = rc->texturePosition().y;
			float w = rc->textureSize().x;
			float h = rc->textureSize().y;

			va.append(sf::Vertex{ sf::Vector2f{x, y}, sf::Vector2f{tx, ty} });
			if (rc->cFlag()) va[va.getVertexCount() - 1].color = rc->color();
			va.append(sf::Vertex{ sf::Vector2f{ x + w, y }, sf::Vector2f{ tx + w, ty } });
			if (rc->cFlag()) va[va.getVertexCount() - 1].color = rc->color();
			va.append(sf::Vertex{ sf::Vector2f{ x + w, y + h }, sf::Vector2f{ tx + w, ty + h } });
			if (rc->cFlag()) va[va.getVertexCount() - 1].color = rc->color();
			va.append(sf::Vertex{ sf::Vector2f{ x, y + h }, sf::Vector2f{ tx, ty + h } });
			if (rc->cFlag()) va[va.getVertexCount() - 1].color = rc->color();
		}

		// Add TextComponents

		sz = AutoListScene<TextComponent>::alsCurrentSize();
		for (int i = 0; i < sz; ++i)
		{
			auto txc = AutoListScene<TextComponent>::alsCurrentGet(i);
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
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else m_window->setView(m_view);
				
				for (auto &vad : pr.second)
				{
					if (vad.va.getVertexCount() == 0) continue;
					if (vad.modified)
					{
						m_states.transform = vad.transform;
					}
					else m_states.transform = sf::Transform{};
					m_states.texture = pr.first;
					m_window->draw(vad.va, m_states);
				}
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

			// Draw tilemap

			if (m_tilemapLayer == layerName)
			{
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else m_window->setView(m_view);
				m_window->draw(m_tilemapSprite);
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

		// Prepare renderTexture if tilemap data is present

		if (event->tilesetData)
		{
			m_tilemapTexture.loadFromFile(event->tilesetData->texture);

			int mapX = event->tilemapSize.x;
			int mapY = event->tilemapSize.y;
			int tileX = event->tilesetData->tileSize.x;
			int tileY = event->tilesetData->tileSize.y;

			m_tilemapRenderTexture.create(mapX * tileX, mapY * tileY);
			m_tilemapRenderTexture.clear();
			sf::VertexArray va;
			va.setPrimitiveType(sf::Quads);

			for (int i = 0; i < event->tilemap.size(); ++i)
			{
				int tVal = event->tilemap[i];
				int x = (i % mapX) * tileX;
				int y = (i / mapX) * tileY;
				int tx = (tVal % (event->tilesetData->textureSize.x / tileX)) * tileX;
				int ty = (tVal / (event->tilesetData->textureSize.x / tileX)) * tileY;

				va.append(sf::Vertex{ sf::Vector2f{(float)x, (float)y}, sf::Vector2f{(float)tx, (float)ty} });
				va.append(sf::Vertex{ sf::Vector2f{ (float)x + (float)tileX, (float)y },
					sf::Vector2f{ (float)tx + (float)tileX, (float)ty } });
				va.append(sf::Vertex{ sf::Vector2f{ (float)x + (float)tileX, (float)y + (float)tileY },
					sf::Vector2f{ (float)tx + (float)tileX, (float)ty + (float)tileY } });
				va.append(sf::Vertex{ sf::Vector2f{ (float)x, (float)y + (float)tileY },
					sf::Vector2f{ (float)tx, (float)ty + (float)tileY } });
			}

			m_states.texture = &m_tilemapTexture;
			m_tilemapRenderTexture.draw(va, m_states);
			m_tilemapRenderTexture.display();

			m_tilemapSprite.setTexture(m_tilemapRenderTexture.getTexture(), true);

			m_tilemapLayer = event->tilemapLayer;
		}
		else
		{
			m_tilemapLayer = "";
		}
	}

	void Renderer::onSetCenter(const SetViewCenterEvent *event)
	{
		m_view.setCenter(event->x, event->y);
	}
}
