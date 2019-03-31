#include "Animator.h"
#include "AnimationComponent.h"

namespace Core
{
	void Animator::execute()
	{
		auto sz = AutoListScene<AnimationComponent>::alsCurrentSize();
		for (size_t i = 0; i < sz; ++i)
		{
			auto ac = AutoListScene<AnimationComponent>::alsCurrentGet(i);
			if (!ac->active() || !ac->playing()) continue;

			auto anim = &ac->m_animation[ac->m_currentAnimationIndex];
			ac->m_frameCounter = ++ac->m_frameCounter % anim->frameDelay;
			if (0 == ac->m_frameCounter)
			{
				ac->m_currentFrameIndex = ++ac->m_currentFrameIndex % anim->frame.size();
				if (0 == ac->m_currentFrameIndex && !anim->loop) ac->stop();
				else ac->setCurrentRenderFrame();
			}
		}
	}
}