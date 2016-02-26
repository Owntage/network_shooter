#include "actor.h"

bool Actor::hasUpdates(int systemID)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->hasUpdate(systemID)) return true;
	}
	return false;
}

bool Actor::hasEvents()
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->globalEvents.size() > 0) return true;
	}
	return false;
}

ActorUpdate Actor::getUpdates(int systemID)
{
	ActorUpdate result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->hasUpdate(systemID))
		{
			result.updates.push_back((*it)->getUpdate(systemID));
		}
	}
	return result;
}

std::vector<std::shared_ptr<Event> > Actor::getGlobalEvents()
{
	std::vector<std::shared_ptr<Event> > result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		while((*it)->globalEvents.size() > 0)
		{
			result.push_back((*it)->globalEvents.back());
			(*it)->globalEvents.pop_back();
		}
	}
	return result;
}

void Actor::onEvent(const Event& event)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		(*it)->onEvent(event);
	}
}

void Actor::approve(std::string component, int systemID, int number)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->getName() == component)  (*it)->approve(systemID, number);
	}
}



