#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "icomponent.h"
#include "world.h"
#include <string>

struct PhysicsEvent : Event
{
	PhysicsEvent(int actorID, int otherActorID, std::string otherType, bool release) : 
		Event("physics", false, actorID),
		otherActorID(otherActorID),
		otherType(otherType),
		release(release)
	{}
	int otherActorID;
	bool release;
	std::string otherType;
};

struct PhysicsComponent : IComponent
{
	PhysicsComponent();
	~PhysicsComponent();
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	int thisActorID;
	ContactData contactData;
	bool up, down, left, right;
	float speed;
	float density;
	float friction;
	float restitution;
	bool wasMoving;
	b2Body* body;

};

#endif