#ifndef EVENT_H
#define EVENT_H

#include <string>

struct Event
{
	Event(std::string name, bool global, int actorID): name(name), global(global), actorID(actorID) {}
	Event(std::string name) : name(name), global(true), actorID(0) {}
	virtual ~Event() {}
	std::string name;
	bool global;
	int actorID;
};

#endif