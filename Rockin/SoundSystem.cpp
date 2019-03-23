#include "SoundSystem.h"
#include <algorithm>

namespace Core
{
	SoundSystem::SoundSystem()
	{
		// Create Channels

		registerFunc(this, &SoundSystem::onSound);
		for (int i = 0; i < m_totalChannels; ++i)
		{
			m_channel.emplace_back("", sf::Sound{}, 0);
		}

		// Load sound buffers

		m_buffer["Laser"] = sf::SoundBuffer{};
		m_buffer["Laser"].loadFromFile("Assets/Sounds/Laser.wav");
		m_buffer["Boom"] = sf::SoundBuffer{};
		m_buffer["Boom"].loadFromFile("Assets/Sounds/Boom.wav");
	}

	void SoundSystem::execute()
	{
		for (const auto &e : m_event)
		{
			// Unused channel?

			auto iter = std::find_if(std::begin(m_channel), std::end(m_channel), [](const Channel &ch)
			{
				return ch.sound.getStatus() == sf::Sound::Status::Stopped;
			});
			if (iter != std::end(m_channel))
			{
				playSound(e, iter);
				continue;
			}

			// Lower priority channel?

			iter = std::find_if(std::begin(m_channel), std::end(m_channel), [&](const Channel &ch)
			{
				return ch.priority < e.priority;
			});
			if (iter != std::end(m_channel))
			{
				playSound(e, iter);
				continue;
			}

			// Replace same sound?

			iter = std::find_if(std::begin(m_channel), std::end(m_channel), [&](const Channel &ch)
			{
				return ch.tag == e.tag && !ch.sound.getLoop();
			});
			if (iter != std::end(m_channel))
			{
				playSound(e, iter);
				continue;
			}

			// No play.
		}
		m_event.clear();
	}

	void SoundSystem::playSound(const SoundEvent &e, std::vector<Channel>::iterator &p)
	{
		p->priority = e.priority;
		p->sound.setBuffer(m_buffer[e.tag]);
		p->sound.setVolume(e.volume);
		p->sound.setLoop(e.loop);
		p->sound.play();
	}

	void SoundSystem::onSound(const SoundEvent *event)
	{
		// No duplicate sounds on a single frame

		auto iter = std::find_if(std::begin(m_event), std::end(m_event), [=](const SoundEvent &e)
		{
			return e.tag == event->tag;
		});

		if (iter != std::end(m_event)) return;

		m_event.push_back(*event);
	}
}