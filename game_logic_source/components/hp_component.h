#ifndef HP_COMPONENT_H
#define HP_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"

struct HpEvent : Event
{
	HpEvent(float delta, int actorID) : 
		Event("hp", false, actorID),
		delta(delta)
	{}
	float delta;
};

struct HpUpdate : ComponentUpdate
{
	HpUpdate(float currentHp, float maxHp, int number) : 
		ComponentUpdate("hp"),
		currentHp(currentHp),
		maxHp(maxHp)
	{
		number = number;
	}
	HpUpdate() {}
	float currentHp;
	float maxHp;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, HpUpdate& u)
	{
		s << u.currentHp << u.maxHp;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, HpUpdate& u)
	{
		s >> u.currentHp >> u.maxHp;
		return s;
	}
};

struct HpComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	float currentHp;
	float maxHp;
};

#endif