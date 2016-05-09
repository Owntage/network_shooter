#include "physics_component.h"
#include "move_event.h"
#include "move_update.h"
#include "coord_event.h"

PhysicsComponent::PhysicsComponent() :
	up(false),
	down(false),
	left(false),
	right(false),
	wasMoving(false),
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
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
		contactData.actorID = thisActorID;
	}
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
		if(body->GetLinearVelocity().LengthSquared() > 0)
		{
			currentDataNumber++;
			wasMoving = true;
		}
		else
		{
			if(wasMoving)
			{
				currentDataNumber++;
				wasMoving = false;
			}
		}
		direction.Normalize();
		if(body->GetLinearVelocity().LengthSquared() < speed * speed)
		{
			body->ApplyForceToCenter(60.0 * direction, true);
		}
		
		if(contactData.hit)
		{
			contactData.hit = false;
			localEvents.push_back(std::make_shared<PhysicsEvent>(thisActorID, 
				contactData.otherActorID, 
				contactData.otherType,
				contactData.release));
		}

	}
	if(event.name == "set_coords")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		body->SetTransform(b2Vec2(coordEvent.x, coordEvent.y), body->GetAngle());
	}
	if(event.name == "set_scale")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		b2Body* newBody;
		if(body->GetType() != b2_dynamicBody)
		{
			newBody = World::getInstance()->createStaticBody(body->GetPosition().x, body->GetPosition().y, coordEvent.x, coordEvent.y);
			World::getInstance()->destroyBody(body);
			body = newBody;
			World::getInstance()->setFriction(body, friction);
			World::getInstance()->setDensity(body, density);
			World::getInstance()->setRestitution(body, restitution);
			World::getInstance()->setContactData(body, contactData);
		}
		//deal with dynamic bodies...
		
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
		return lastSystemApproved[systemID] < currentDataNumber;
	}
}

std::string PhysicsComponent::getName()
{
	return "move";
}

std::shared_ptr<ComponentUpdate> PhysicsComponent::getUpdate(int systemID)
{
	std::shared_ptr<MoveUpdate> result = std::make_shared<MoveUpdate>(body->GetPosition().x, body->GetPosition().y,
		body->GetLinearVelocity().x, body->GetLinearVelocity().y, World::getInstance()->getTime());
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
	result->density = density;
	result->restitution = restitution;
	result->friction = friction;
	World::setDensity(result->body, density);
	World::setRestitution(result->body, restitution);
	World::setFriction(result->body, friction);
	result->contactData.type = id;
	World::setContactData(result->body, result->contactData);
	return result;
}

