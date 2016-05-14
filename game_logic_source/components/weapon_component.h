#ifndef WEAPON_COMPONENT_H
#define WEAPON_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"
#include "../event.h"
#include "../properties_loader.h"
#include <singleton.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <vector>

#define WEAPON_PROPERTIES_FILE "res/weapon_properties.xml"

struct WeaponPropertiesVisitor : XmlVisitor
{
	std::string getPropertiesFilename() const;
	void onVisit(const boost::property_tree::ptree& tree, std::string nodeName) const;
};


struct WeaponDef
{
	float period;
	float reloadTime;
	float dispersion; //in radians
	int holders;
	int bulletsPerHolder;
	std::string bulletType;
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
	WeaponComponent();
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	friend class WeaponPropertiesVisitor;
	static std::map<std::string, WeaponDef> weaponDefinitions;
	std::vector<WeaponDef> weapons;
	int currentWeapon;
	int thisActorID;
};

#endif