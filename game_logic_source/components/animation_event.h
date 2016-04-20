#ifndef ANIMATION_EVENT_H
#define ANIMATION_EVENT_H

#include "../event.h"
#include "animation_component.h"

struct AnimationEvent : Event
{
	AnimationEvent(int layerNumber, LayerState layerState, int actorID) : 
		Event("animation", false, actorID), 
		layerNumber(layerNumber),
		layerState(layerState)
	{}
	int layerNumber;
	LayerState layerState;

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