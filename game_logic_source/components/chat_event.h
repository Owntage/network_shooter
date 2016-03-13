#ifndef CHAT_EVENT_H
#define CHAT_EVENT_H

#include "../event.h"
#include <string>

struct ChatEvent : Event
{
	ChatEvent(std::string message, int actorID) : Event("chat", false, actorID), message(message), actorID(actorID) {}
	std::string message;
	int actorID;
};

#endif