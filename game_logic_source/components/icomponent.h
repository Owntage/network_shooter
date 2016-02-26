#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "../event.h"
#include "component_update.h"
#include <memory>
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

struct IComponent
{
	virtual ~IComponent() {}
	virtual void onEvent(const Event& event) = 0;
	virtual bool hasUpdate(int systemID) = 0;
	virtual void approve(int systemID, int number) = 0;
	virtual std::string getName() = 0;
	virtual std::shared_ptr<ComponentUpdate>  getUpdate(int syatemID) = 0;
	virtual std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree) = 0;
	std::vector<std::shared_ptr<Event> > globalEvents;
	std::vector<std::shared_ptr<Event> > localEvents;
};

#endif