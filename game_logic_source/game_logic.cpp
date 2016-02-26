#include "game_logic.h"

void GameLogic::onEvent(const Event& event)
{
	if(event.global)
	{
		for(auto it = actors.begin(); it != actors.end(); it++)
		{
			it->second->onEvent(event);
			while(it->second->getGlobalEvents().size() > 0)
			{
				auto globalEvent = it->second->getGlobalEvents().back();
				globalEvent->global = true;
				onEvent(*globalEvent);
				it->second->getGlobalEvents().pop_back();
			}
		}
	}
	else
	{
		actors[event.actorID]->onEvent(event);
		while(actors[event.actorID]->getGlobalEvents().size() > 0)
		{
			auto globalEvent = actors[event.actorID]->getGlobalEvents().back();
			globalEvent->global = true;
			onEvent(*globalEvent);
			actors[event.actorID]->getGlobalEvents().pop_back();
		}
	}
}

void GameLogic::approve(int actorID, std::string component, int systemID, int number)
{
	if(actors.find(actorID) != actors.end())
	{
		actors[actorID]->approve(component, systemID, number);
	}
}

std::vector<std::shared_ptr<ActorUpdate> > GameLogic::getUpdates(int systemID)
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		if(it->second->hasUpdates(systemID))
		{
			result.push_back(std::make_shared<ActorUpdate>(it->second->getUpdates(systemID)));
			result.back()->actorID = it->first;
		}
		
	}
	return result;
}

int GameLogic::registerSystem()
{
	systemCount++;
	return systemCount;
}

int GameLogic::createActor(std::string actorID)
{
	actorCount++;
	actors[actorCount] = actorFactory.createActor(actorID);
	return actorCount;
}

void GameLogic::destroyActor(int actorID)
{
	actors.erase(actors.find(actorID));
}





