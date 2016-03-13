#ifndef NETWORK_LOGIC_H
#define NETWORK_LOGIC_H

#include <actor_update.h>
#include <SFML/Network.hpp>
#include <string>
#include <render_system.h>
#include "controller.h"

#define CONTROLLER_SYSTEM_ID 0

struct NetworkLogic
{
	NetworkLogic(sf::IpAddress address, unsigned short port, std::string actorType, Controller& controller, RenderSystem& renderSystem);
	void sendEvents();
	std::vector<std::shared_ptr<ActorUpdate> > receiveUpdates();
	int getActorID();
private:
	void refreshTimeout();
	Controller& controller;
	RenderSystem& renderSystem;
	sf::IpAddress address;
	sf::UdpSocket receivingSocket;
	sf::UdpSocket sendingSocket;
	unsigned short localPort;
	unsigned short port;
	int uniqueID;
	int actorID;
	std::string actorType;
	sf::Packet packet;
	enum class State
	{
		GETTING_UNIQUE_ID,
		CREATING_ACTOR,
		RECEIVING_UPDATES
	};
	State state;
};

#endif