#include "server.h"
#include <components/move_component.h>
#include <string>
#include <iostream>
#include <components/move_event.h>
#include <components/chat_component.h>


GameServer::GameServer(GameLogic& gameLogic, unsigned short port): uniqueCounter(0), gameLogic(gameLogic)
{
	socket.bind(port);
	socket.setNonBlocking();
}

void GameServer::receiveEvents()
{
	Packet packet;
	IpAddress remoteAddress;
	while(socket.receive(packet, remoteAddress))
	{
		unsigned short remotePort;
		std::string type;
		packet >> remotePort >> type;
		if(type == "id")
		{
			std::cout << "id sent" << std::endl;
			sendUniqueID(remoteAddress);
		}
		else if(type == "create")
		{
			std::string actorID;
			int uniqueID;
			packet >> uniqueID >> actorID;
			createActor(remoteAddress, uniqueID, actorID);
		}
		else if(type == "refresh_timeout")
		{
			int uniqueID;
			packet >> uniqueID;
			refreshTimeout(uniqueID);
		}
		else if(type == "event")
		{
			receiveGameEvent(packet, remoteAddress);
		}
		else if(type == "approve")
		{

			int actorID;
			int uniqueID;
			std::string updateName;
			int updateNumber;
			packet >> uniqueID >> actorID >> updateName >> updateNumber;
			//gameLogic.approve(actorID, updateName,
			if(clients.find(uniqueID) != clients.end())
			{
				gameLogic.approve(actorID, updateName, clients[uniqueID].systemID, updateNumber);
			}
				
		}

		
	}
	
}

void GameServer::receiveGameEvent(Packet& packet, IpAddress& address)
{
	int uniqueID;
	packet >> uniqueID;
	if(clients.find(uniqueID) != clients.end())
	{
		
		Event event;
		packet >> event;
		event.actorID = clients[uniqueID].gameLogicID;
		//std::cout << "received game event: " << event.name << std::endl;
		if(clients[uniqueID].eventNumbers.find(event.name) == clients[uniqueID].eventNumbers.end())
		{
			clients[uniqueID].eventNumbers[event.name] = 0;
		}
		if(event.number > clients[uniqueID].eventNumbers[event.name])
		{
			clients[uniqueID].eventNumbers[event.name] = event.number;
			if(event.name == "move")
			{
				MoveEvent moveEvent;
				(Event&) moveEvent = event;
				packet >> moveEvent;
				gameLogic.onEvent(moveEvent);
			}
			if(event.name == "chat")
			{
				ChatEvent chatEvent;
				(Event&) chatEvent = event;
				packet >> chatEvent;
				gameLogic.onEvent(chatEvent);
			}
		}
		packet.reset();
		packet << "approve" << event.name << event.number;
		socket.send(address, packet);
	}
	else
	{
		std::cout << "unique id is wrong" << std::endl;
	}
}

void GameServer::sendUpdates()
{
	
	//checking for timeouts
	std::vector<int> timedoutClients;
	for(auto it = clients.begin(); it != clients.end(); it++)
	{
		it->second.time += 1.0f / 60.0f;
		if(it->second.time >= 10.0f)
		{
			std::cout << "client timed out:" << std::endl;
			timedoutClients.push_back(it->first);
		}
	}
	for(auto it = timedoutClients.begin(); it != timedoutClients.end(); it++)
	{
		destroyActor(*it);
	}
	for(auto client_it = clients.begin(); client_it != clients.end(); client_it++)
	{
		auto updates = gameLogic.getUpdates(client_it->second.systemID);
		for(auto actor_it = updates.begin(); actor_it != updates.end(); actor_it++)
		{
			for(auto component_it = (*actor_it)->updates.begin(); component_it != (*actor_it)->updates.end(); component_it++)
			{
				Packet packet;
				packet << "update" << *(*component_it);

				if((*component_it)->name == "move") packet << (MoveUpdate&) *(*component_it);
				if((*component_it)->name == "chat") packet << (ChatUpdate&) *(*component_it);

				socket.send(client_it->second.address, packet);
			}
		}
	}
}

void GameServer::destroyActor(int uniqueID)
{
	gameLogic.destroyActor(clients[uniqueID].gameLogicID);
	clients.erase(clients.find(uniqueID));
}

void GameServer::refreshTimeout(int uniqueID)
{
	clients[uniqueID].time = 0.0f;
}

void GameServer::sendUniqueID(IpAddress& remoteAddress)
{
	Packet packet;
	packet << "id" << uniqueCounter;
	uniqueCounter++;
	socket.send(remoteAddress, packet);
}

void GameServer::sendSuccessfulCreation(int uniqueID)
{
	std::cout << "sending successful creation" << std::endl;
	Packet packet;
	packet << "create" << "success" << clients[uniqueID].gameLogicID;
	socket.send(clients[uniqueID].address, packet);
}

void GameServer::createActor(IpAddress& remoteAddress, int uniqueID, std::string actorID)
{
	if(clients.find(uniqueID) != clients.end())
	{
		sendSuccessfulCreation(uniqueID);
	}
	else
	{
		std::cout << "creating actor" << std::endl;
		ClientData data;
		data.address = remoteAddress;
		data.gameLogicID = gameLogic.createActor(actorID);
		data.systemID = gameLogic.registerSystem();
		data.time = 0.0f;
		clients[uniqueID] = data;
		sendSuccessfulCreation(uniqueID);
	}
}

void GameServer::approve(int uniqueID, int actorID, std::string component, int number)
{
	gameLogic.approve(actorID, component, clients[uniqueID].systemID, number);
}