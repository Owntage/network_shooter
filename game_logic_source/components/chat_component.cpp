#include "chat_component.h"

void ChatComponent::onEvent(const Event& event)
{
	//todo
}

bool ChatComponent::hasUpdate(int systemID)
{
	if(systemUpdates.find(systemID) == systemUpdates.end())
	{
		return true;
	}
	return lastSystemApproved[systemID] != currentSystemNumber[systemID];
}

std::string ChatComponent::getName()
{
	return "chat";
}

std::shared_ptr<ComponentUpdate> ChatComponent::getUpdate(int systemID)
{
	std::shared_ptr<ChatUpdate> result = std::make_shared<ChatUpdate>();
	/*
	if(systemUpdates.find(systemID) == systemUpdates.end())
	{
		result->messageCount = 0;
	}
	else
	{
		result->messageCount = chatHistory.size();
	}
	int counter = 0;
	while(result->messageCount < chatHistory.size())
	{
		result->messageCount++;
		result->messages.push_back(chatHistory[chatHistory.size() - counter]);
		counter++;
	}
	if(systemUpdates.find(systemID) != systemUpdates.end())
	{
		result->number = systemUpdates[systemID].number;
	}
	else
	{
		result->number = 0;
	}
	systemUpdates[systemID] = *result;
	systemUpdateNumbers[systemID] = result->number;
	*/
	return result;
	//todo peredelat'
}

std::shared_ptr<IComponent> ChatComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<ChatComponent>();
}
//todo add to factory

