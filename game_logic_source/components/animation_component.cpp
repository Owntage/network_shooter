#include "animation_component.h"
#include "animation_event.h"
#include "animation_update.h"
#include "boost/foreach.hpp"

void AnimationComponent::onEvent(const Event& event)
{
	if(event.name == "animation")
	{
		AnimationEvent& animationEvent = (AnimationEvent&) event;
		if(states.find(animationEvent.animationState) != states.end())
		{
			currentState = animationEvent.animationState;
			currentDataNumber++;
		}
	}
}

bool AnimationComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end()) return true;
	return lastSystemApproved[systemID] != currentDataNumber;
}

std::shared_ptr<ComponentUpdate> AnimationComponent::getUpdate(int systemID)
{
	std::shared_ptr<AnimationUpdate> result = std::make_shared<AnimationUpdate>(currentState, currentDataNumber);
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		//result->states = states;
		for(auto it = states.begin(); it != states.end(); it++)
		{
			result->states.push_back(it->second);
		}
	}
	return result;
}

std::shared_ptr<IComponent> AnimationComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	std::shared_ptr<AnimationComponent> result = std::make_shared<AnimationComponent>();
	bool first = true;
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "state")
		{
			AnimationState animationState;
			animationState.delay = 0.3f;
			animationState.stateName = "default";
			
			BOOST_FOREACH(auto& state_v, v.second)
			{
				if(state_v.first == "name")
				{
					animationState.stateName = state_v.second.get_value<std::string>();
				}
				if(state_v.first == "delay")
				{
					animationState.delay = state_v.second.get_value<float>();
				}
				if(state_v.first == "image")
				{
					animationState.images.push_back(state_v.second.get_value<std::string>());
				}
			}
			if(first)
			{
				first = false;
				result->currentState = animationState.stateName;
			}
			result->states[animationState.stateName] = animationState;
		}
	}

	return result;
}