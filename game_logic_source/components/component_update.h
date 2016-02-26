#ifndef COMPONENT_UPDATE_H
#define COMPONENT_UPDATE_H

#include <string>

struct ComponentUpdate
{
	ComponentUpdate(std::string name): name(name), number(0) {}
	virtual ~ComponentUpdate() {}
	std::string name;
	int number;
};

#endif