#ifndef ANIMATION_EVENT_H
#define ANIMATION_EVENT_H

#include "../event.h"

struct AnimationEvent : Event
{
	AnimationEvent(int layerNumber, std::string animationState, bool isActive, int actorID) : 
		Event("animation", false, actorID), 
		layerNumber(layerNumber),
		animationState(animationState),
		isActive(isActive)
	{}
	std::string animationState;
	int layerNumber;
	bool isActive;

	/*
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
	*/
};

#endif