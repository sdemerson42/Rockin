#pragma once

#include "CoreComponentBase.h"
#include "SFML\Graphics.hpp"
#include <string>
#include <vector>
#include "AutoListScene.h"

namespace Core
{
	/*
	AnimationComponent - Stores animation state and receives requests to start or stop animations
	received from scripts. State changes in the current animation are controlled by the Animation system.
	*/
	class AnimationComponent : public CoreComponentBase, public sde::AutoList<AnimationComponent>, public AutoListScene<AnimationComponent>
	{
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
		// Inline accessors
		Animation &curentAnimation()
		{
			return m_animation[m_currentAnimationIndex];
		}
		int frameCounter() const
		{
			return m_frameCounter;
		}
		void setFrameCounter(int val)
		{
			m_frameCounter = val;
		}
		int currentFrameIndex() const
		{
			return m_currentFrameIndex;
		}
		void setCurrentFrameIndex(int val)
		{
			m_currentFrameIndex = val;
		}
	private:
		std::vector<Animation> m_animation;
		int m_currentAnimationIndex;
		int m_currentFrameIndex;
		AnimationState m_currentState;
		int m_frameCounter;
		bool m_defaultAnimation;
	};
}
