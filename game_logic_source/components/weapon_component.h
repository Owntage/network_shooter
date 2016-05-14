#ifndef WEAPON_COMPONENT_H
#define WEAPON_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"
#include "../event.h"
#include <singleton.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define WEAPON_PROPERTIES_FILE "res/weapon_properties.xml"

struct WeaponProperties : Singleton<WeaponProperties>
{
	WeaponProperties()
	{
		boost::property_tree::read_xml(WEAPON_PROPERTIES_FILE, properties);
		properties = properties.get_child("weapons");
	}
	boost::property_tree::ptree properties;
};

struct WeaponEvent : Event
{
	WeaponEvent(int actorID, std::string weaponID) : 
		Event("weapon", false, actorID),
		weaponID(weaponID)
	{}
	std::string weaponID;
};

struct WeaponComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
};

#endif