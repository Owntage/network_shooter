#ifndef DEATHMATCH_COMPONENT_H
#define DEATHMATCH_COMPONENT_H

#include "icomponent.h"
#include "frag_count_component.h"
#include <vector>
#include <utility>

struct DeathmatchUpdate : ComponentUpdate
{
	enum class GameState
	{
		RUNNING,
		TIMEOUT
	};

	DeathmatchUpdate(std::vector<std::pair<std::string, int> >& actors, GameState state, int number) :
		ComponentUpdate("deathmatch"),
		actors(actors),
		state(state)
	{
		this->number = number;
	}
	DeathmatchUpdate() {}
	std::vector<std::pair<std::string, int> > actors;
	
	GameState state;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, DeathmatchUpdate& u)
	{
		s << (int) u.actors.size();
		for(int i = 0; i < u.actors.size(); i++)
		{
			s << u.actors[i].first;
			s << u.actors[i].second;
		}
		s << (int) u.state;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, DeathmatchUpdate& u)
	{
		int size;
		s >> size;
		u.actors.resize(size);
		for(int i = 0; i < size; i++)
		{
			s >> u.actors[i].first;
			s >> u.actors[i].second;
		}
		s >> (int&) u.state;
		return s;
	}
};

struct DeathmatchComponent : IComponent
{
	DeathmatchComponent() :
		currentTime(0.0f)
	{}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	int totalRequests;
	int requestsHandled;
	std::vector<std::pair<std::string, int> > aliveActors;
	std::vector<std::pair<std::string, int> > tempAliveActors;
	float roundTime;
	float currentTime;
};

#endif