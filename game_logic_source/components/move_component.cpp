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
		if(movingRight || movingLeft || movingUp || movingDown)
		{
			currentDataNumber++;
		}
	}
}

std::string MoveComponent::getName()
{
	return "move";
}

bool MoveComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end()) return true;
	return lastSystemApproved[systemID] != currentDataNumber;
}



std::shared_ptr<ComponentUpdate> MoveComponent::getUpdate(int systemID)
{
	std::shared_ptr<MoveUpdate> result = std::make_shared<MoveUpdate>(x, y);
	result->number = currentDataNumber;
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