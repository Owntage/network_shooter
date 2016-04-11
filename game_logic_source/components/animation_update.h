#ifndef ANIMATION_UPDATE_H
#define ANIMATION_UPDATE_H

#include "component_update.h"
#include <string>
#include <vector>
#include "animation_component.h"

struct AnimationUpdate : ComponentUpdate
{
	AnimationUpdate(std::string animationState, int number) : ComponentUpdate("animation"), animationState(animationState)
	{
		this->number = number;
	}
	AnimationUpdate() {}
	std::string animationState;
	std::vector<AnimationState> states;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, AnimationUpdate& u)
	{
		s << u.animationState << (int) u.states.size();
		for(int i = 0; i < u.states.size(); i++)
		{
			s << u.states[i];
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, AnimationUpdate& u)
	{
		s >> u.animationState;
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			AnimationState temp;
			s >> temp;
			u.states.push_back(temp);
		}
		return s;
	}
};

#endif