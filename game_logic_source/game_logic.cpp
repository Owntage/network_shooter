#include "game_logic.h"
#include "delete_update.h"
#include <iostream>

void GameLogic::onEvent(const Event& event)
{
	if(event.global)
	{
		for(auto it = actors.begin(); it != actors.end(); it++)
		{
			it->second->onEvent(event);
			
			auto globalEvents = it->second->getGlobalEvents();
			while(globalEvents.size() > 0)
			{
				auto globalEvent = globalEvents.back();
				globalEvent->global = true;
				onEvent(*globalEvent);
				globalEvents.pop_back();
			}
			
			auto localEvents = it->second->getLocalEvents();
			while(localEvents.size() > 0)
			{
				auto localEvent = localEvents.back();
				localEvent->global = false;
				localEvent->actorID = it->first;
				onEvent(*localEvent);
				localEvents.pop_back();
			}
		}
	}
	else
	{
		actors[event.actorID]->onEvent(event);
		auto globalEvents = actors[event.actorID]->getGlobalEvents();
		while(globalEvents.size() > 0)
		{
			auto globalEvent = globalEvents.back();
			globalEvent->global = true;
			onEvent(*globalEvent);
			globalEvents.pop_back();
		}

		auto localEvents = actors[event.actorID]->getLocalEvents();
		while(localEvents.size() > 0)
		{
			auto localEvent = localEvents.back();
			localEvent->global = false;
			localEvent->actorID = event.actorID;
			onEvent(*localEvent);
			localEvents.pop_back();
		}
	}
}

void GameLogic::approve(int actorID, std::string component, int systemID, int number)
{
	if(component == "delete")
	{
		deletedActorApproves[systemID] = std::max(deletedActorApproves[systemID], number);
		return;
	}
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
			result.back()->actor = it->second;
			for(auto component_update = result.back()->updates.begin(); component_update != result.back()->updates.end(); component_update++)
			{
				(*component_update)->actorID = it->first;
			}
		}
		
	}

	if(deletedActorApproves[systemID] != deletedActors.size() - 1)
	{
		auto deleteUpdate = std::make_shared<DeleteUpdate>(deletedActors.size() - 1);
		for(int i = deletedActorApproves[systemID] + 1; i < deletedActors.size(); i++)
		{
			//std::cout << "deleted actor: " << deletedActors[i] << std::endl;
			deleteUpdate->deletedActors.push_back(deletedActors[i]);
		}
		deleteUpdate->actorID = -1;
		auto deleteActorUpdate = std::make_shared<ActorUpdate>();
		deleteActorUpdate->actorID = -1;
		deleteActorUpdate->updates.push_back(deleteUpdate);
		result.push_back(deleteActorUpdate);
	}
	return result;
}

std::vector<std::shared_ptr<ActorUpdate> > GameLogic::getUpdates(int systemID, float x, float y, float updateRadius)
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		if(it->second->hasUpdates(systemID))
		{
			if(it->second->hasCoords && sqrt(pow(it->second->x - x, 2) + pow(it->second->y - y, 2)) >= updateRadius)
			{
				continue;
			}
			result.push_back(std::make_shared<ActorUpdate>(it->second->getUpdates(systemID)));
			result.back()->actorID = it->first;
			result.back()->actor = it->second;
			for(auto component_update = result.back()->updates.begin(); component_update != result.back()->updates.end(); component_update++)
			{
				(*component_update)->actorID = it->first;
			}
		}

	}

	if(deletedActorApproves[systemID] != deletedActors.size() - 1)
	{
		auto deleteUpdate = std::make_shared<DeleteUpdate>(deletedActors.size() - 1);
		for(int i = deletedActorApproves[systemID] + 1; i < deletedActors.size(); i++)
		{
			//std::cout << "deleted actor: " << deletedActors[i] << std::endl;
			deleteUpdate->deletedActors.push_back(deletedActors[i]);
		}
		deleteUpdate->actorID = -1;
		auto deleteActorUpdate = std::make_shared<ActorUpdate>();
		deleteActorUpdate->actorID = -1;
		deleteActorUpdate->updates.push_back(deleteUpdate);
		result.push_back(deleteActorUpdate);
	}
	return result;
}

int GameLogic::registerSystem()
{
	systemCount++;
	deletedActorApproves[systemCount] = deletedActors.size() - 1;
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
	deletedActors.push_back(actorID);
	actors.erase(actors.find(actorID));
}





