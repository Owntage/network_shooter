#include "weapon_component.h"

void WeaponComponent::onEvent(const Event& event)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool WeaponComponent::hasUpdate(int systemID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::string WeaponComponent::getName()
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<ComponentUpdate> WeaponComponent::getUpdate(int syatemID)
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::shared_ptr<IComponent> WeaponComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	throw std::logic_error("The method or operation is not implemented.");
}
