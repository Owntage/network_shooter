#include "animation_swapping_component.h"
#include "move_event.h"
#include "string_event.h"
#include "animation_event.h"

void AnimationSwappingComponent::onEvent(const Event& event)
{
	if(event.name == "move")
	{
		//std::cout << "animation swapping component received move event" << std::endl;
		const MoveEvent& moveEvent = (const MoveEvent&) event;
		bool moving = moveEvent.up || moveEvent.down || moveEvent.left || moveEvent.right;
		if(moving)
		{
			localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(runningTexture, true, 45.0f), 0));
		}
		else
		{
			localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(normalTexture, true, 0.0f), 0));
		}
	}
}

bool AnimationSwappingComponent::hasUpdate(int systemID)
{
	return false;
}
std::string AnimationSwappingComponent::getName()
{
	return "animation_swapping";
}

std::shared_ptr<ComponentUpdate> AnimationSwappingComponent::getUpdate(int syatemID)
{
	return std::shared_ptr<ComponentUpdate>();
}

std::shared_ptr<IComponent> AnimationSwappingComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<AnimationSwappingComponent>();
	result->normalTexture = tree.get("normal_texture", "normal_texture");
	result->runningTexture = tree.get("running_texture", "running_texture");
	return result;
}

