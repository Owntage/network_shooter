#include "move_component.h"
#include "move_event.h"

void MoveComponent::onEvent(const Event& event)
{
	if(event.name == "move")
	{
		const MoveEvent& moveEvent = static_cast<const MoveEvent&>(event);
		movingLeft = moveEvent.left;
		movingRight = moveEvent.right;
		movingUp = moveEvent.up;
		movingDown = moveEvent.down;
	}
	else if(event.name == "timer")
	{
		if(movingRight) x += speed / 60.0f;
		if(movingLeft) x -= speed / 60.0f;
		if(movingUp) y -= speed / 60.0f;
		if(movingDown) y += speed / 60.0f;
	}
}

std::string MoveComponent::getName()
{
	return "move";
}

bool MoveComponent::hasUpdate(int systemID)
{
	if(systemUpdates.find(systemID) == systemUpdates.end()) return true;
	return lastSystemApproved[systemID] != currentSystemNumber[systemID];
}



std::shared_ptr<ComponentUpdate> MoveComponent::getUpdate(int systemID)
{
	std::shared_ptr<MoveUpdate> result = std::make_shared<MoveUpdate>(x, y);
	if(systemUpdates.find(systemID) != systemUpdates.end())
	{
		if(result->x != systemUpdates[systemID].x || result->y != systemUpdates[systemID].y)
		{
			currentSystemNumber[systemID]++;
		}
	}
	else
	{
		currentSystemNumber[systemID] = 1;
		
	}
	systemUpdates[systemID] = *result;
	systemUpdates[systemID].number = currentSystemNumber[systemID];
	return result;
}

std::shared_ptr<IComponent> MoveComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	std::shared_ptr<MoveComponent> result = std::make_shared<MoveComponent>();
	result->x = tree.get("x", 0.0f);
	result->y = tree.get("y", 0.0f);
	result->speed = tree.get("speed", 1.0f);
	result->movingLeft = false;
	result->movingRight = false;
	result->movingUp = false;
	result->movingDown = false;
	return result;
}