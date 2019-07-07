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

			auto anim = &ac->curentAnimation();
			ac->setFrameCounter((ac->frameCounter() + 1) % anim->frameDelay);
			if (0 == ac->frameCounter())
			{
				ac->setCurrentFrameIndex((ac->currentFrameIndex() + 1) % anim->frame.size());
				if (0 == ac->currentFrameIndex() && !anim->loop) ac->stop();
				else ac->setCurrentRenderFrame();
			}
		}
	}
}