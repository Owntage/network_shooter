#include "network_logic.h"
#include <string>
#include <iostream>
#include <components/move_update.h>
#include <components/chat_component.h>


NetworkLogic::NetworkLogic(sf::IpAddress address, unsigned short port, std::string actorType, Controller& controller) : address(address), port(port), state(State::GETTING_UNIQUE_ID), actorType(actorType), controller(controller), lastApprove(-1)
{
	receivingSocket.bind(sf::UdpSocket::AnyPort);
	localPort = receivingSocket.getLocalPort();
}

void NetworkLogic::refreshTimeout()
{
	packet.clear();
	packet << localPort << std::string("refresh_timeout") << uniqueID;
	sendingSocket.send(packet, address, port);
}

void NetworkLogic::sendEvents()
{
	
	switch(state)
	{
	case State::GETTING_UNIQUE_ID:
		packet.clear();
		packet << localPort << std::string("id");
		sendingSocket.send(packet, address, port);
		break;
	case State::CREATING_ACTOR:
		std::cout << "requesting for creation of the actor" << std::endl;
		packet.clear();
		packet << localPort << std::string("create") << uniqueID << actorType;
		if(sendingSocket.send(packet, address, port) != sf::Socket::Done)
		{
			std::cout << "failed to send request for creating" << std::endl;
		}
		else
		{
			std::cout << "cretion request sent successfuly" << std::endl;
		}
		break;
	case State::RECEIVING_UPDATES:
		refreshTimeout();
		auto events = controller.getGameEvents(0);
		for(auto it = events.begin(); it != events.end(); it++)
		{
			if((*it)->name == "move")
			{
				MoveEvent& moveEvent = static_cast<MoveEvent&>(*(*it));
				if(moveEvent.number > lastApprove)
				{
					packet.clear();
					packet << localPort << std::string("event") << uniqueID << (Event&) moveEvent << moveEvent;
					sendingSocket.send(packet, address, port);
				}
			}
		}
		break;
	}
}

std::vector<std::shared_ptr<ActorUpdate> > NetworkLogic::receiveUpdates()
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	std::map<int, std::shared_ptr<ActorUpdate> > mappedUpdates;
	
	sf::SocketSelector selector;
	selector.add(receivingSocket);
	packet.clear();
	
	while(true)
	{

		if(selector.wait(sf::milliseconds(1)))
		{
			unsigned short temp;
			receivingSocket.receive(packet, address, temp);
			std::string packetType;
			std::string creationResult;
			packet >> packetType;
			switch(state)
			{
			case State::GETTING_UNIQUE_ID:
				if(packetType == "id")
				{
					std::cout << "successful connection to the server" << std::endl;
					packet >> uniqueID;
					state = State::CREATING_ACTOR;
					packet.clear();
					packet << localPort << std::string("create") << uniqueID << actorType;
					if(sendingSocket.send(packet, address, port) != sf::Socket::Done)
					{
						std::cout << "failed to send request for creating" << std::endl;
					}
			else
			{
				std::cout << "cretion request sent successfuly" << std::endl;
			}
				}
				break;
			case State::CREATING_ACTOR:
				if(packetType == "create")
				{
					std::cout << "actor has been created" << std::endl;
			
					packet >> creationResult;
					if(creationResult == "success")
					{
						packet >> actorID;
						controller.setActorID(actorID);
						state = State::RECEIVING_UPDATES;
					}
				}
				break;
			case State::RECEIVING_UPDATES:
				if(packetType == "update")
				{
					ComponentUpdate componentUpdate;
					packet >> componentUpdate;
					if(componentUpdate.name == "move")
					{
						std::shared_ptr<MoveUpdate> moveUpdate = std::make_shared<MoveUpdate>();
						(ComponentUpdate&) *moveUpdate = componentUpdate;
						packet >> *moveUpdate;
						if(mappedUpdates.find(componentUpdate.actorID) == mappedUpdates.end())
						{
							mappedUpdates[componentUpdate.actorID] = std::make_shared<ActorUpdate>();
						}
						mappedUpdates[componentUpdate.actorID]->actorID = componentUpdate.actorID;
						mappedUpdates[componentUpdate.actorID]->updates.push_back(moveUpdate);				
					}
				}
				else if(packetType == "approve")
				{
					std::string approveType;
					packet >> approveType;
					if(approveType == "move")
					{
						std::cout << "received move approve" << std::endl;
						int number;
						packet >> number;
						lastApprove = number;
						controller.approve(CONTROLLER_SYSTEM_ID, number);
					}
				}
				break;
			}
	
		}
		else
		{
			break;
		}
	}
	
	
	
	for(auto it = mappedUpdates.begin(); it != mappedUpdates.end(); it++)
	{
		result.push_back(it->second);
	}
	return result;
}

int NetworkLogic::getActorID()
{
	return actorID;
}