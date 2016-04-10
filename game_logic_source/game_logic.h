#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>
#include <memory>
#include <map>
#include "actor_factory.h"
#include "actor_update.h"
#include "event.h"


struct GameLogic
{
	GameLogic(ActorFactory& actorFactory): actorFactory(actorFactory), systemCount(0), actorCount(0) {}
	void onEvent(const Event& event);
	void approve(int actorID, std::string component, int systemID, int number);
	std::vector<std::shared_ptr<ActorUpdate> > getUpdates(int systemID);
	int registerSystem();
	int createActor(std::string id);
	void destroyActor(int number);
private:
	ActorFactory& actorFactory;
	int systemCount;
	int actorCount;
	std::map<int, std::shared_ptr<Actor> > actors;
	std::map<int, int> deletedActorApproves;
	std::vector<int> deletedActors;
};

#endif