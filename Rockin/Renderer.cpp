#include "Renderer.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CoreEntity.h"
#include <iostream>
#include "TextComponent.h"
#include "TilesetData.h"
#include "ParticlesComponent.h"

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

		// Add Particles

		sz = AutoListScene<ParticlesComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto pc = AutoListScene<ParticlesComponent>::alsCurrentGet(i);
			if ((!pc->active() && !pc->despawnPersist()) || pc->count() == 0) continue;
			auto &ld = m_layerMap[pc->layer()];
			ld.particle.push_back(sf::VertexArray{});
			auto &va = ld.particle.back();
			auto &color = pc->color();

			for (int i = 0; i < pc->count(); ++i)
			{
				auto &pData = pc->particleData()[i];

				// Exclude particles that are out of frame

				if (!ld.isStatic)
				{
					float entL = pData.position.x;
					float entR = entL + pData.size;
					float entT = pData.position.y;
					float entB = entT + pData.size;

					if (entR < viewL || entL > viewR ||
						entB < viewT || entT > viewB) continue;
				}
				
				va.setPrimitiveType(sf::PrimitiveType::Quads);

				float x = pData.position.x;
				float y = pData.position.y;
				int size = pData.size;

				va.append(sf::Vertex{ sf::Vector2f{ x, y }, color });
				va.append(sf::Vertex{ sf::Vector2f{ x + size, y }, color });
				va.append(sf::Vertex{ sf::Vector2f{ x + size, y + size }, color });
				va.append(sf::Vertex{ sf::Vector2f{ x, y + size }, color });
			}	
		}


		// Add TextComponents

		sz = AutoListScene<TextComponent>::alsCurrentSize();
		for (int i = 0; i < sz; ++i)
		{
			auto txc = AutoListScene<TextComponent>::alsCurrentGet(i);
			if (!txc->active()) continue;
			const auto &txcParentPos = txc->parent()->position();
			txc->text().setPosition(txcParentPos.x + txc->offset().x, txcParentPos.y + txc->offset().y);
			m_layerMap[txc->layer()].text.push_back(&txc->text());
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

			// Draw particles

			for (auto &va : ld.particle)
			{
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else m_window->setView(m_view);
				m_window->draw(va);
			}
			ld.particle.clear();

			// Draw tilemap

			if (m_tilemapLayer == layerName)
			{
				if (ld.isStatic) m_window->setView(m_window->getDefaultView());
				else
				{
					m_window->setView(m_view);
					buildTilemapTexture(viewL, viewR, viewT, viewB);
				}
				m_window->draw(m_tilemapSprite);
			}
		}
		m_window->display();
	}

	void Renderer::buildTilemapTexture(float vl, float vr, float vt, float vb, bool force)
	{
		if (!force)
		{
			// Do we need a rebuild?
			if (vl >= m_viewL && vr <= m_viewR && vt >= m_viewT && vb <= m_viewB) return;
		}

		int mapX = m_tilemapSize.x;
		int mapY = m_tilemapSize.y;
		int tileX = m_tileSize.x;
		int tileY = m_tileSize.y;

		m_tilemapRenderTexture.create(m_tilemapCellSize, m_tilemapCellSize);
		m_tilemapRenderTexture.clear(sf::Color::Transparent);
		sf::VertexArray va;
		va.setPrimitiveType(sf::Quads);

		// Calculate tile quad

		int tl = (int)m_view.getCenter().x - (m_tilemapCellSize / 2);
		float offsetX = m_view.getCenter().x - ((float)m_tilemapCellSize / 2.0f);
		tl /= tileX;
		offsetX -= (float)tl * (float)tileX;
		int tr = tl + m_tilemapCellSize / tileX;
		int tt = (int)m_view.getCenter().y - (m_tilemapCellSize / 2);
		float offsetY = m_view.getCenter().y - ((float)m_tilemapCellSize / 2.0f);
		tt /= tileY;
		offsetY -= (float)tt * (float)tileY;
		int tb = tt + m_tilemapCellSize / tileY;

		int x = 0;
		int y = 0;
		for (int j = tt; j < tb; ++j)
		{
			for (int i = tl; i < tr; ++i)
			{
				if (i >= 0 && i < mapX && j >=0 && j < mapY)
				{
					int tVal = m_tilemap[j * mapX + i];
					int tx = (tVal % (m_tilesetTextureSize.x / tileX)) * tileX;
					int ty = (tVal / (m_tilesetTextureSize.x / tileX)) * tileY;

					va.append(sf::Vertex{ sf::Vector2f{ (float)x, (float)y }, sf::Vector2f{ (float)tx, (float)ty } });
					va.append(sf::Vertex{ sf::Vector2f{ (float)x + (float)tileX, (float)y },
						sf::Vector2f{ (float)tx + (float)tileX, (float)ty } });
					va.append(sf::Vertex{ sf::Vector2f{ (float)x + (float)tileX, (float)y + (float)tileY },
						sf::Vector2f{ (float)tx + (float)tileX, (float)ty + (float)tileY } });
					va.append(sf::Vertex{ sf::Vector2f{ (float)x, (float)y + (float)tileY },
						sf::Vector2f{ (float)tx, (float)ty + (float)tileY } });
				}

				x = (x + tileX) % m_tilemapCellSize;
				if (x == 0) y += tileY;
			}
		}

		m_states.texture = &m_tilemapTexture;
		m_tilemapRenderTexture.draw(va, m_states);
		m_tilemapRenderTexture.display();

		m_tilemapSprite.setTexture(m_tilemapRenderTexture.getTexture(), true);
		m_tilemapSprite.setPosition((int)m_view.getCenter().x - (m_tilemapCellSize / 2) - offsetX, (int)m_view.getCenter().y - (m_tilemapCellSize / 2) - offsetY);

		m_viewL = m_tilemapSprite.getPosition().x;
		m_viewR = m_viewL + m_tilemapCellSize;
		m_viewT = m_tilemapSprite.getPosition().y;
		m_viewB = m_viewT + m_tilemapCellSize;
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

		/*
		 Prepare renderTexture if tilemap data is present
		 NOTE: sf::RenderTexture represents a composite texture that is
		 drawn once at scene construction, then displayed each frame.
		 Processing tileset data in this way reduces computation time
		 by many orders of magnitude.
		 */

		if (event->tilesetData)
		{
			m_tilemapTexture.loadFromFile(event->tilesetData->texture);
			m_tilemapSize = event->tilemapSize;
			m_tileSize = event->tilesetData->tileSize;
			m_tilemap = event->tilemap;
			m_tilemapLayer = event->tilemapLayer;
			m_tilesetTextureSize = event->tilesetData->textureSize;
			// TEMP
			m_tilemapBoundsTile = 0;

			m_view.setCenter(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f);
			float viewL = m_view.getCenter().x - m_view.getSize().x / 2.0f;
			float viewR = m_view.getCenter().x + m_view.getSize().x / 2.0f;
			float viewT = m_view.getCenter().y - m_view.getSize().y / 2.0f;
			float viewB = m_view.getCenter().y + m_view.getSize().y / 2.0f;
			buildTilemapTexture(viewL, viewR, viewT, viewB, true);
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
