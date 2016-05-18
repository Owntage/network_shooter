#include "actor_factory.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "components/chat_component.h"
#include "components/move_component.h"
#include "components/animation_component.h"
#include "components/tile_component.h"
#include "components/physics_component.h"
#include "components/animation_swapping_component.h"
#include "components/render_component.h"
#include "components/timed_deletion_component.h"
#include "components/contact_deletion_component.h"
#include "components/spawner_component.h"
#include "components/weapon_component.h"
#include "components/hp_component.h"
#include "components/damage_dealer_component.h"
#include "components/bullet_component.h"

ActorFactory::ActorFactory(std::string propertiesPath)
{
	boost::property_tree::ptree properties;
	read_xml(propertiesPath, properties);

		BOOST_FOREACH(auto& v, properties.get_child("properties"))
		{
			actors[v.first] = v.second;
		}
			
}

std::shared_ptr<Actor> ActorFactory::createActor(std::string id)
{
	std::shared_ptr<Actor> result;
	if(actors.find(id) == actors.end()) return result;

	result = std::make_shared<Actor>();

	BOOST_FOREACH(auto& v, actors.find(id)->second)
	{
		std::shared_ptr<IComponent> component;
		if(v.first == "move") component = std::make_shared<MoveComponent>();
		if(v.first == "chat") component = std::make_shared<ChatComponent>();
		if(v.first == "animation") component = std::make_shared<AnimationComponent>();
		if(v.first == "tile") component = std::make_shared<TileComponent>();
		if(v.first == "physics") component = std::make_shared<PhysicsComponent>();
		if(v.first == "animation_swapping") component = std::make_shared<AnimationSwappingComponent>();
		if(v.first == "render") component = std::make_shared<RenderComponent>();
		if(v.first == "timed_deletion") component = std::make_shared<TimedDeletionComponent>();
		if(v.first == "contact_deletion") component = std::make_shared<ContactDeletionComponent>();
		if(v.first == "spawner") component = std::make_shared<SpawnerComponent>();
		if(v.first == "weapon") component = std::make_shared<WeaponComponent>();
		if(v.first == "hp") component = std::make_shared<HpComponent>();
		if(v.first == "damage_dealer") component = std::make_shared<DamageDealerComponent>();
		if(v.first == "bullet") component = std::make_shared<BulletComponent>();
		result->components.push_back(component->loadFromXml(v.second));
	}
	return result;
}