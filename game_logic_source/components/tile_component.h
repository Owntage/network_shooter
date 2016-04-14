#ifndef TILE_COMPONENT_H
#define TILE_COMPONENT_H

#include "icomponent.h"

struct TileComponent : IComponent
{
	virtual void onEvent(const Event& event);
	virtual bool hasUpdate(int systemID);
	virtual std::string getName();
	virtual std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	virtual std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string image;
	float x;
	float y;

};

#endif