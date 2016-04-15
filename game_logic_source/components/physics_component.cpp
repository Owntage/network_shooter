#include "physics_component.h"
#include "move_event.h"
#include "move_update.h"

PhysicsComponent::PhysicsComponent() :
	up(false),
	down(false),
	left(false),
	right(false),
	speed(3.0f)
{
	body = nullptr;
}

PhysicsComponent::~PhysicsComponent()
{
	if(body != nullptr)
	{
		World::getInstance()->destroyBody(body);
	}
	
}

void PhysicsComponent::onEvent(const Event& event)
{
	if(event.name == "move")
	{
		const MoveEvent& moveEvent = (const MoveEvent&) event;
		up = moveEvent.up;
		down = moveEvent.down;
		left = moveEvent.left;
		right = moveEvent.right;
	}
	if(event.name == "timer")
	{
		b2Vec2 direction(0, 0);
		if(right) direction += b2Vec2(1, 0);
		if(left) direction += b2Vec2(-1, 0);
		if(up) direction += b2Vec2(0, -1);
		if(down) direction += b2Vec2(0, 1);
		if(direction.LengthSquared() != 0)
		{
			currentDataNumber++;
		}
		direction.Normalize();
		if(body->GetLinearVelocity().LengthSquared() < speed * speed)
		{
			body->ApplyForceToCenter(60.0 * direction, true);
		}
		//here should be handled hit
	}
}

bool PhysicsComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		return true;
	}
	else
	{
		return lastSystemApproved[systemID] < systemID;
	}
}

std::string PhysicsComponent::getName()
{
	return "physics";
}

std::shared_ptr<ComponentUpdate> PhysicsComponent::getUpdate(int syatemID)
{
	std::shared_ptr<MoveUpdate> result = std::make_shared<MoveUpdate>(body->GetPosition().x, body->GetPosition().y);
	result->number = currentDataNumber;
	return result;
}

std::shared_ptr<IComponent> PhysicsComponent::loadFromXml(const boost::property_tree::ptree& tree)
{

	std::shared_ptr<PhysicsComponent> result = std::make_shared<PhysicsComponent>();
	double x = tree.get("x", 0.0);
	double y = tree.get("y", 0.0);
	result->speed = tree.get("speed", 0.0);
	std::string type = tree.get<std::string>("type");
	std::string id = tree.get<std::string>("id");
	if(type == "static")
	{
		double width = tree.get("width", 0.5);
		double height = tree.get("height", 0.5);
		result->body = World::getInstance()->createStaticBody(x, y, width, height);
	}
	if(type == "dynamic")
	{
		result->body = World::getInstance()->createDynamicBody(x, y);
	}
	if(type == "circle")
	{
		double radius = tree.get("radius", 0.5);
		result->body = World::getInstance()->createDynamicCircle(x, y, radius);
	}
	double density = tree.get("density", 1.0);
	double restitution = tree.get("restitution", 0.2);
	double friction = tree.get("friction", 0.5);
	World::setDensity(result->body, density);
	World::setRestitution(result->body, restitution);
	World::setFriction(result->body, friction);
	result->contactData.id = id;
	World::setContactData(result->body, result->contactData);
	return result;
}

