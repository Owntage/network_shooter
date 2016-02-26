#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include "actor_update.h"
#include "components/icomponent.h"
#include "event.h"


struct Actor
{
	bool hasUpdates(int systemID);
	bool hasEvents();
	ActorUpdate getUpdates(int systemID);
	std::vector<std::shared_ptr<Event> > getGlobalEvents();
	void onEvent(const Event& event);
	void approve(std::string component, int systemID, int number);
	friend class ActorFactory;
	Actor() {}
private:
	std::vector<std::shared_ptr<IComponent> > components;
};

#endif