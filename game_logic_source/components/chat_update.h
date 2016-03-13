#ifndef CHAT_UPDATE_H
#define CHAT_UPDATE_H

#include "component_update.h"
#include <string>
#include <vector>

struct ChatUpdate : ComponentUpdate
{
	ChatUpdate(std::string lastMessage, int position) : ComponentUpdate("chat"), rangeBegin(position), rangeEnd(position)
	{
		messages.push_back(lastMessage);
	}
	ChatUpdate() : ComponentUpdate("chat") {}
	int rangeBegin;
	int rangeEnd;
	std::vector<std::string> messages;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, ChatUpdate& u)
	{
		s << u.rangeBegin << u.rangeEnd;
		for(int i = 0; i < u.rangeEnd - u.rangeBegin + 1; i++)
		{
			s << u.messages[i];
		}
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, ChatUpdate& u)
	{
		s >> u.rangeBegin >> u.rangeEnd;
		u.messages.resize(u.rangeEnd - u.rangeBegin + 1);
		for(int i = 0; i < u.rangeEnd - u.rangeBegin + 1; i++)
		{
			s >> u.messages[i];
		}
		return s;
	}
};

#endif