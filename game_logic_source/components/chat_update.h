#ifndef CHAT_UPDATE_H
#define CHAT_UPDATE_H

#include "component_update.h"
#include <string>
#include <vector>

struct ChatUpdate : ComponentUpdate
{
	ChatUpdate(std::string lastMessage, int messageCount) :	ComponentUpdate("chat"), messageCount(messageCount)
	{
		messages.push_back(lastMessage);
	}
	ChatUpdate(std::vector<std::string> messages, int messageCount) : ComponentUpdate("chat"), messages(messages), messageCount(messageCount) {}
	ChatUpdate(): ComponentUpdate("chat") {}
	std::vector<std::string> messages;
	int messageCount;
};

#endif