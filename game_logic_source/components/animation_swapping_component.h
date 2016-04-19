#ifndef ANIMATION_SWAPPING_COMPONENT_H
#define ANIMATION_SWAPPING_COMPONENT_H

#include "icomponent.h"

struct AnimationSwappingComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string runningTexture;
	std::string normalTexture;
};

#endif