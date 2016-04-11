#ifndef ANIMATION_EVENT_H
#define ANIMATION_EVENT_H

#include "../event.h"

struct AnimationEvent : Event
{
	AnimationEvent(std::string animationState, int actorID) : Event("animation", false, actorID), animationState(animationState) {}
	std::string animationState;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, AnimationEvent& e)
	{
		s << e.animationState;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, AnimationEvent& e)
	{
		s >> e.animationState;
		return s;
	}
};

#endif