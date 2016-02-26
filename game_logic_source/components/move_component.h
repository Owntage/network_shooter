#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "icomponent.h"
#include "move_update.h"
#include <map>

struct MoveComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	void approve(int systemID, int number);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int systemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	bool movingLeft;
	bool movingRight;
	bool movingUp;
	bool movingDown;
	float speed;
	float x, y;
	std::map<int, MoveUpdate> systemUpdates;
	std::map<int, bool> systemApprovals;
};

#endif