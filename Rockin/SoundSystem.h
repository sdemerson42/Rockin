#pragma once

#include "SFML\Audio.hpp"
#include "sde.h"
#include <vector>
#include <map>
#include "Events.h"

namespace Core
{
	class SoundSystem : public sde::ISystem
	{
	public:
		SoundSystem();
		void execute() override;
	private:
		const int m_totalChannels = 4;
		sf::Music m_music;
		struct Channel
		{
			Channel(const std::string &tag, const sf::Sound sound, int priority) :
				tag{ tag }, sound{ sound }, priority{ priority }
			{}
			std::string tag;
			sf::Sound sound;
			int priority;
		};
		std::vector<Channel> m_channel;
		std::map<std::string, sf::SoundBuffer> m_buffer;
		std::vector<SoundEvent> m_event;

		void playSound(const SoundEvent &, std::vector<Channel>::iterator &p);
		void onSound(const SoundEvent *);
		void onMusic(const MusicEvent *);
	};
}
