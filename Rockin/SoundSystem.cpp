#include "SoundSystem.h"
#include <algorithm>
#include <fstream>

namespace Core
{
	SoundSystem::SoundSystem()
	{
		// Create Channels

		registerFunc(this, &SoundSystem::onSound);
		registerFunc(this, &SoundSystem::onMusic);

		for (int i = 0; i < m_totalChannels; ++i)
		{
			m_channel.emplace_back("", sf::Sound{}, 0);
		}

		// Load sound buffers

		std::ifstream ifs{ "Data/Sounds.dat" };
		std::string tag;
		std::string fName;

		while (true)
		{
			ifs >> tag;
			if (!ifs) break;
			ifs >> fName;
			m_buffer[tag] = sf::SoundBuffer{};
			m_buffer[tag].loadFromFile(fName);
		}
	}

	void SoundSystem::execute()
	{
		for (const auto &e : m_event)
		{
			// Stopping sound?

			if (e.stop)
			{
				auto iter = std::find_if(std::begin(m_channel), std::end(m_channel), [&](const Channel &ch)
				{
					return ch.tag == e.tag;
				});

				if (iter != std::end(m_channel))
				{
					iter->sound.stop();
				}
				continue;
			}

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

	void SoundSystem::onMusic(const MusicEvent *event)
	{
		if (event->action == 2)
		{
			m_music.stop();
			return;
		}
		else if (event->action == 1)
		{
			m_music.pause();
			return;
		}
		m_music.openFromFile(event->fileName);
		m_music.setVolume(event->volume);
		m_music.setLoop(event->loop);
		m_music.play();
	}
}