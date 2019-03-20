#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "CoreEntity.h"
#include <algorithm>

namespace Core
{
	AnimationComponent::AnimationComponent(CoreEntity *parent) :
		CoreComponentBase{ parent }, m_currentAnimationIndex{ -1 },
		m_currentFrameIndex{ 0 }, m_currentState{ AnimationState::Stopped }, m_frameCounter{ 0 },
		m_defaultAnimation{ false }
	{
	}

	void AnimationComponent::initialize()
	{
		if (!m_defaultAnimation) stop();
		else play("default");
	}

	void AnimationComponent::addAnimation(const Animation &a)
	{
		m_animation.push_back(a);
		if (a.name == "default") 
		{
			m_defaultAnimation = true;
			play(a.name);
		}
	}
	void AnimationComponent::play(const std::string &name)
	{
		if (m_animation.size() == 0) return;
		if (m_currentAnimationIndex != -1 && m_animation[m_currentAnimationIndex].name == name 
			&& m_currentState == AnimationState::Playing) return;

		auto iter = std::find_if(std::begin(m_animation), std::end(m_animation), [&](const Animation &a)
		{
			return a.name == name;
		});
		if (iter != std::end(m_animation))
		{
			m_currentAnimationIndex = iter - std::begin(m_animation);
			m_currentFrameIndex = 0;
			m_currentState = AnimationState::Playing;
			m_frameCounter = 0;
			setCurrentRenderFrame();
		}
	}
	void AnimationComponent::stop()
	{
		m_currentState = AnimationState::Stopped;
		m_frameCounter = 0;
	}
	bool AnimationComponent::playing()
	{
		return m_currentState == AnimationState::Playing;
	}

	void AnimationComponent::setCurrentRenderFrame()
	{
		auto rc = parent()->getComponent<RenderComponent>();
		if (!rc) return;
		auto p = &m_animation[m_currentAnimationIndex];
		rc->m_tPos = p->frame[m_currentFrameIndex].position;
		rc->m_tSize = p->frame[m_currentFrameIndex].size;
	}
}