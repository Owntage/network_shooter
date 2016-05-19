#include "nickname_component.h"
#include "string_event.h"

void NicknameComponent::onRequest(const Request& request)
{
	if(request.name == "get_nickname")
	{
		request.callback(StringEvent("", 0, nickname));
	}
}

void NicknameComponent::onEvent(const Event& event)
{
	if(event.name == "set_nickname")
	{
		const StringEvent& stringEvent = (const StringEvent&) event;
		nickname = stringEvent.str;
	}
}

bool NicknameComponent::hasUpdate(int systemID)
{
	return lastSystemApproved.find(systemID) == lastSystemApproved.end();
}

std::string NicknameComponent::getName()
{
	return "nickname";
}

std::shared_ptr<ComponentUpdate> NicknameComponent::getUpdate(int syatemID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<IComponent> NicknameComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	throw std::logic_error("The method or operation is not implemented.");
}

