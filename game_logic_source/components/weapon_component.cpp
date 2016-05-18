#include "weapon_component.h"
#include "physics_component.h"
#include "coord_event.h"
#include "create_event.h"
#include "damage_dealer_component.h"


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
	weaponDef.weaponTexture = tree.get<std::string>("texture");
	weaponDef.damage = tree.get("damage", 10.0f);
	weaponDef.bullets = tree.get("bullets", 1);
	

	WeaponComponent::weaponDefinitions[nodeName] = weaponDef;
}

WeaponComponent::WeaponComponent() :
	currentWeapon(0),
	isShooting(false),
	state(WeaponUpdate::WeaponState::CHANGE)
{
	if(weaponDefinitions.size() == 0)
	{
		WeaponPropertiesVisitor visitor;
		walkXml(visitor.getPropertiesFilename(), visitor);
	}
	currentDataNumber = 0;
}

void WeaponComponent::shoot()
{
	if(weapons.size() > 0)
	{
		for(int i = 0; i < weapons[currentWeapon].bullets; i++)
		{
			auto request = std::make_shared<Request>("angle", false, thisActorID, [this](const Event& event)
			{
				const AngleEvent& angleEvent = (const AngleEvent&) event;
				float angle = angleEvent.angle;
				float rand_num = std::rand() % 100;
				rand_num /= 100.0f;
				rand_num -= 0.5f;
				angle += rand_num * weapons[currentWeapon].dispersion;
				auto coordRequest = std::make_shared<Request>("coords", false, thisActorID, [this, angle](const Event& event)
				{
					const CoordEvent& coordEvent = (const CoordEvent&) event;
					auto createEvent = std::make_shared<CreateEvent>();
					createEvent->type = weapons[currentWeapon].bulletType;
					createEvent->actorID = thisActorID;

					float rand_num = std::rand() % 100;
					rand_num /= 100.0f;
					rand_num -= 0.5f;

					auto newCoordEvent = std::make_shared<CoordEvent>("set_coords", 0, coordEvent.x + 0.8 * cos(angle), coordEvent.y + 0.8 * sin(angle));
					float speed = weapons[currentWeapon].bulletSpeed;
					speed += speed * 0.3 * rand_num;
					auto speedCoordEvent = std::make_shared<CoordEvent>("set_speed", 0, speed * cos(angle), speed * sin(angle));
					auto damageEvent = std::make_shared<DamageEvent>(
						weapons[currentWeapon].damage, 
						weapons[currentWeapon].bulletType,
						thisActorID, 0);
					createEvent->events.push_back(newCoordEvent);
					createEvent->events.push_back(speedCoordEvent);
					createEvent->events.push_back(damageEvent);
					localEvents.push_back(createEvent);



				});
				requests.push_back(coordRequest);
			});
			requests.push_back(request);
		}
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
					state = WeaponUpdate::WeaponState::SHOOT;
					currentDataNumber++;
				}
				else
				{
					if(weaponData[currentWeapon].holdersSpent < weapons[currentWeapon].holders)
					{
						weaponData[currentWeapon].shotsMade = 0;
						weaponData[currentWeapon].timeSinceReload = 0;
						weaponData[currentWeapon].timeSinceShot = 0;
						weaponData[currentWeapon].holdersSpent++;
						state = WeaponUpdate::WeaponState::RELOAD;
						currentDataNumber++;
					}
				}
			}
		}
	}
}

bool WeaponComponent::hasUpdate(int systemID)
{
	
	if(weapons.size() == 0)
	{
		return false;
	}

	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		return true;
	}
	
	
	return lastSystemApproved[systemID] < currentDataNumber;
}

std::string WeaponComponent::getName()
{
	return "weapon";
}

std::shared_ptr<ComponentUpdate> WeaponComponent::getUpdate(int syatemID)
{
	auto result = std::make_shared<WeaponUpdate>(weapons[currentWeapon], weaponData[currentWeapon], state);
	result->number = currentDataNumber;
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
