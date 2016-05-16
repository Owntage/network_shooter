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
	weaponDef.bulletSpeed = tree.get("bullet_speed", 10.0f);
	WeaponComponent::weaponDefinitions[nodeName] = weaponDef;
}

WeaponComponent::WeaponComponent() :
	currentWeapon(0),
	isShooting(false)
{
	if(weaponDefinitions.size() == 0)
	{
		WeaponPropertiesVisitor visitor;
		walkXml(visitor.getPropertiesFilename(), visitor);
	}
}

void WeaponComponent::shoot()
{
	if(weapons.size() > 0)
	{
		auto request = std::make_shared<Request>("angle", false, thisActorID, [this](const Event& event)
		{
			const AngleEvent& angleEvent = (const AngleEvent&) event;
			float angle = angleEvent.angle;
			auto coordRequest = std::make_shared<Request>("coords", false, thisActorID, [this, angle](const Event& event)
			{
				const CoordEvent& coordEvent = (const CoordEvent&) event;
				auto createEvent = std::make_shared<CreateEvent>();
				createEvent->type = weapons[currentWeapon].bulletType;
				createEvent->actorID = thisActorID;

				auto newCoordEvent = std::make_shared<CoordEvent>("set_coords", 0, coordEvent.x + 1.5 * cos(angle), coordEvent.y + 1.5 * sin(angle));
				auto speedCoordEvent = std::make_shared<CoordEvent>("set_speed", 0, weapons[currentWeapon].bulletSpeed * cos(angle), weapons[currentWeapon].bulletSpeed * sin(angle));
				createEvent->events.push_back(newCoordEvent);
				createEvent->events.push_back(speedCoordEvent);
				localEvents.push_back(createEvent);

			});
			requests.push_back(coordRequest);
		});
		requests.push_back(request);
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
			WeaponData w;
			weaponData.push_back(w);
		}
	}
	if(event.name == "shooting_begin")
	{
		isShooting = true;
	}
	if(event.name == "shooting_end")
	{
		isShooting = false;
	}
	if(event.name == "timer")
	{
		if(weapons.size() > 0)
		{
			weaponData[currentWeapon].timeSinceReload += 1.0f / 60.0f;
			weaponData[currentWeapon].timeSinceShot += 1.0f / 60.0f;
			if(weaponData[currentWeapon].timeSinceShot > weapons[currentWeapon].period &&
				weaponData[currentWeapon].timeSinceReload > weapons[currentWeapon].reloadTime &&
				isShooting)
			{
				if(weaponData[currentWeapon].shotsMade < weapons[currentWeapon].bulletsPerHolder)
				{
					shoot();
					weaponData[currentWeapon].shotsMade++;
					weaponData[currentWeapon].timeSinceShot = 0;
				}
				else
				{
					if(weaponData[currentWeapon].holdersSpent < weapons[currentWeapon].holders)
					{
						weaponData[currentWeapon].shotsMade = 0;
						weaponData[currentWeapon].timeSinceReload = 0;
						weaponData[currentWeapon].timeSinceShot = 0;
						weaponData[currentWeapon].holdersSpent++;
					}
				}
			}
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
				WeaponData w;
				result->weaponData.push_back(w);
				result->weapons.push_back(weaponDefinitions[v.second.get_value<std::string>()]);
			}
			
		}
	}
	std::cout << "finished loading weapon" << std::endl;
	return result;
}
