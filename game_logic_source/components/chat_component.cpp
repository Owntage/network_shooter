#include "chat_component.h"
#include <iostream>

std::vector<std::string> ChatComponent::chatHistory;

void ChatComponent::onEvent(const Event& event)
{
	if(event.name == "chat")
	{
		const ChatEvent& chatEvent = static_cast<const ChatEvent&>(event);
		chatHistory.push_back(chatEvent.message);
		std::cout << "chat component received a message: " << chatEvent.message << std::endl;
	}
}

bool ChatComponent::hasUpdate(int systemID)
{
	
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		lastSystemApproved[systemID] = 0;
		return true;
	}
	return lastSystemApproved[systemID] != chatHistory.size();
}

std::string ChatComponent::getName()
{
	return "chat";
}

std::shared_ptr<ComponentUpdate> ChatComponent::getUpdate(int systemID)
{
	std::shared_ptr<ChatUpdate> result = std::make_shared<ChatUpdate>();
	result->number = chatHistory.size();
	currentSystemNumber[systemID] = chatHistory.size();
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		lastSystemApproved[systemID] = 0;
	}
	result->rangeBegin = lastSystemApproved[systemID];
	result->rangeEnd = chatHistory.size() - 1;
	for(int i = result->rangeBegin; i <= result->rangeEnd; i++)
	{
		result->messages.push_back(chatHistory[i]);
	}
	std::cout << "history size: " << chatHistory.size() << std::endl;
	return result;
}

std::shared_ptr<IComponent> ChatComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<ChatComponent>();
}
//todo add to factory

