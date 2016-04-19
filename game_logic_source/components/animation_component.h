#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "icomponent.h"
#include <vector>
#include <string>
#include <map>

struct AnimationState
{
	std::vector<std::string> images;
	float delay;
	std::string stateName;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, AnimationState& a)
	{
		s << a.stateName << a.delay << (int) a.images.size();
		for(int i = 0; i < a.images.size(); i++)
		{
			s << a.images[i];
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, AnimationState& a)
	{
		s >> a.stateName >> a.delay;
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			std::string temp;
			s >> temp;
			a.images.push_back(temp);
		}
		return s;
	}
};

struct AnimationComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName() {return "animation"; }
	std::shared_ptr<ComponentUpdate> getUpdate(int systemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::map<std::string, AnimationState> states;
	std::vector<std::pair<bool, std::string> > currentLayerStates;
};

#endif