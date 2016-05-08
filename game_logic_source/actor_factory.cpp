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
		result->components.push_back(component->loadFromXml(v.second));
	}
	return result;
}