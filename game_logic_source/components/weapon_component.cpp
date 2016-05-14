#include "weapon_component.h"
#include "physics_component.h"
#include "coord_event.h"
#include "create_event.h"

std::map<std::string, WeaponDef> WeaponComponent::weaponDefinitions;

std::string WeaponPropertiesVisitor::getPropertiesFilename() const
{
	return WEAPON_PROPERTIES_FILE;
}

void WeaponPropertiesVisitor::onVisit(const boost::property_tree::ptree& tree, std::string nodeName) const
{
	WeaponDef weaponDef;
	weaponDef.bulletsPerHolder = tree.get("bullets_per_holder", 10);
	weaponDef.holders = tree.get("holders", 3);
	weaponDef.reloadTime = tree.get("reload_time", 1.0f);
	weaponDef.period = tree.get("period", 0.5f);
	weaponDef.bulletType = tree.get<std::string>("bullet_type");
	weaponDef.dispersion = tree.get("dispersion", 0.3f);
	WeaponComponent::weaponDefinitions[nodeName] = weaponDef;
}

WeaponComponent::WeaponComponent() :
	currentWeapon(0)
{
	if(weaponDefinitions.size() == 0)
	{
		WeaponPropertiesVisitor visitor;
		walkXml(visitor.getPropertiesFilename(), visitor);
	}
}

void WeaponComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
	}
	if(event.name == "new_weapon")
	{
		const WeaponEvent& weaponEvent = (const WeaponEvent&) event;
		if(weaponDefinitions.find(weaponEvent.weaponID) == weaponDefinitions.end())
		{
			std::cout << "weaponID " << weaponEvent.weaponID << " not found" << std::endl;
		}
		else
		{
			weapons.push_back(weaponDefinitions[weaponEvent.weaponID]);
		}
	}
	if(event.name == "shoot")
	{
		std::cout << "shooting..." << std::endl;
		if(weapons.size() > 0)
		{
			Request request("angle", false, thisActorID, [this](const Event& event)
			{
				const AngleEvent& angleEvent = (const AngleEvent&) event;
				float angle = angleEvent.angle;
				Request coordRequest("coords", false, thisActorID, [this, angle](const Event& event)
				{
					const CoordEvent& coordEvent = (const CoordEvent&) event;
					
				});
			});
		}
	}
}

bool WeaponComponent::hasUpdate(int systemID)
{
	return false;
}

std::string WeaponComponent::getName()
{
	return "weapon";
}

std::shared_ptr<ComponentUpdate> WeaponComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> WeaponComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	std::cout << "started loading weapon" << std::endl;
	std::shared_ptr<WeaponComponent> result = std::make_shared<WeaponComponent>();
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "default_weapon")
		{
			std::string weaponID = v.second.get_value<std::string>();
			if(weaponDefinitions.find(weaponID) == weaponDefinitions.end())
			{
				std::cout << "could not find weapon with id " << weaponID << std::endl;
			}
			else
			{
				result->weapons.push_back(weaponDefinitions[v.second.get_value<std::string>()]);
			}
			
		}
	}
	std::cout << "finished loading weapon" << std::endl;
	return result;
}
