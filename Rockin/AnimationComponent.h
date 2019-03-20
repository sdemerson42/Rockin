#pragma once

#include "CoreComponentBase.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <vector>

namespace Core
{
	class AnimationComponent : public CoreComponentBase, public sde::AutoList<AnimationComponent>
	{
		friend class Animator;
	public:
		enum class AnimationState { Playing, Stopped };
		struct Frame
		{
			Frame()
			{}
			Frame(float px, float py, float w, float h) :
				position{ px, py }, size{ w, h }
			{}
			sf::Vector2f position;
			sf::Vector2f size;
		};
		struct Animation
		{
			std::string name;
			std::vector<Frame> frame;
			int frameDelay;
			bool loop;
		};
		void initialize() override;
		AnimationComponent(CoreEntity *parent);
		void addAnimation(const Animation &a);
		void play(const std::string &name);
		void stop();
		bool playing();
		void setCurrentRenderFrame();
	private:
		std::vector<Animation> m_animation;
		int m_currentAnimationIndex;
		int m_currentFrameIndex;
		AnimationState m_currentState;
		int m_frameCounter;
		bool m_defaultAnimation;
	};
}
