#include "deathmatch_component.h"
#include "string_event.h"

void DeathmatchComponent::onEvent(const Event& event)
{
	
	if(event.name == "timer")
	{
		currentTime += 1.0f / 60.0f;
		totalRequests = FragCountComponent::frags.size();
		requestsHandled = 0;
		tempAliveActors.clear();
		for(auto it = FragCountComponent::frags.begin(); it != FragCountComponent::frags.end(); it++)
		{
			int frags = it->second;
			auto request = std::make_shared<Request>("get_nickname", false, it->first, [this, frags]
			(const Event& event)
			{
				if(event.name == "get_nickname")
				{
					const StringEvent& stringEvent = (const StringEvent&) event;
					tempAliveActors.push_back(std::make_pair(stringEvent.str, frags));
				}
				requestsHandled++;
				if(requestsHandled == totalRequests)
				{
					
					if(aliveActors != tempAliveActors)
					{
						currentDataNumber++;
					}
					aliveActors = tempAliveActors;
				}
			});
			requests.push_back(request);
		}
	}
}

bool DeathmatchComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		return true;
	}
	return lastSystemApproved[systemID] < currentDataNumber;
}

std::string DeathmatchComponent::getName()
{
	return "deathmatch";
}

std::shared_ptr<ComponentUpdate> DeathmatchComponent::getUpdate(int syatemID)
{
	std::cout << "alive actors size: " << aliveActors.size() << std::endl;
	return std::make_shared<DeathmatchUpdate>(aliveActors, DeathmatchUpdate::GameState::RUNNING, currentDataNumber);
}

std::shared_ptr<IComponent> DeathmatchComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<DeathmatchComponent>();
}

