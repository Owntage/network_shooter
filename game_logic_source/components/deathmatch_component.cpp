#include "deathmatch_component.h"


void DeathmatchComponent::onEvent(const Event& event)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool DeathmatchComponent::hasUpdate(int systemID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::string DeathmatchComponent::getName()
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<ComponentUpdate> DeathmatchComponent::getUpdate(int syatemID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<IComponent> DeathmatchComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	throw std::logic_error("The method or operation is not implemented.");
}

