#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "icomponent.h"
#include "world.h"

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
	ContactData contactData;
	bool up, down, left, right;
	float speed;
	b2Body* body;

};

#endif