#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include "actor_update.h"
#include "components/icomponent.h"
#include "event.h"
#include "request.h"


struct Actor
{
	Actor() : hasCoords(false) {}
	bool hasUpdates(int systemID);
	bool hasEvents();
	ActorUpdate getUpdates(int systemID);
	std::vector<std::shared_ptr<Event> > getGlobalEvents();
	std::vector<std::shared_ptr<Event> > getLocalEvents();
	std::vector<std::shared_ptr<Request> > getRequests();
	void onEvent(const Event& event);
	void onRequest(const Request& request);
	void approve(std::string component, int systemID, int number);
	friend class ActorFactory;
	
	bool hasCoords;
	float x;
	float y;

private:
	std::vector<std::shared_ptr<IComponent> > components;
};

#endif