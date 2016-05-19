#ifndef NICKNAME_COMPONENT_H
#define NICKNAME_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"

struct NicknameUpdate : ComponentUpdate
{
	NicknameUpdate(std::string nickname) :
		ComponentUpdate("nickname"),
		nickname(nickname)
	{}
	NicknameUpdate() {}
	std::string nickname;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, NicknameUpdate& u)
	{
		s << u.nickname;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, NicknameUpdate& u)
	{
		s >> u.nickname;
		return s;
	}
};



struct NicknameComponent : IComponent
{
	void onRequest(const Request& request);
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string nickname;
};

#endif