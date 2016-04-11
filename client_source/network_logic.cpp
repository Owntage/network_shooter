#include "network_logic.h"
#include <string>
#include <iostream>
#include <components/move_update.h>
#include <components/chat_component.h>
#include <components/animation_update.h>
#include <delete_update.h>


NetworkLogic::NetworkLogic(IpAddress address, std::string actorType, Controller& controller, RenderSystem& renderSystem) : address(address), state(State::GETTING_UNIQUE_ID), actorType(actorType), controller(controller), renderSystem(renderSystem)
{
	//receivingSocket.bind(sf::UdpSocket::AnyPort);
	socket.bind(0);
	localPort = socket.getLocalPort();
	socket.setNonBlocking();
}

void NetworkLogic::refreshTimeout()
{
	packet.reset();
	packet << localPort << "refresh_timeout" << uniqueID;
	socket.send(address, packet);
}

void NetworkLogic::sendEvents()
{
	
	switch(state)
	{
	case State::GETTING_UNIQUE_ID:
		packet.reset();
		packet << localPort << "id";
		socket.send(address, packet);
		break;
	case State::CREATING_ACTOR:
		std::cout << "requesting for creation of the actor" << std::endl;
		packet.reset();
		packet << localPort << "create" << uniqueID << actorType;
		if(socket.send(address, packet))
		{
			std::cout << "cretion request sent successfuly" << std::endl;
		}
		else
		{
			std::cout << "failed to send request for creating" << std::endl;
		}
		break;
	case State::RECEIVING_UPDATES:
		refreshTimeout();
		auto events = controller.getGameEvents();
		for(auto it = events.begin(); it != events.end(); it++)
		{
			if((*it)->name == "move")
			{
				MoveEvent& moveEvent = static_cast<MoveEvent&>(*(*it));
				packet.reset();
				packet << localPort << "event" << uniqueID << (Event&) moveEvent << moveEvent;
				socket.send(address, packet);
			}
			if((*it)->name == "chat")
			{
				ChatEvent& chatEvent = static_cast<ChatEvent&>(*(*it));
				packet.reset();
				packet << localPort << "event" << uniqueID << (Event&) chatEvent << chatEvent;
				socket.send(address, packet);
			}
		}
		break;
	}
}

std::vector<std::shared_ptr<ActorUpdate> > NetworkLogic::receiveUpdates()
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	std::map<int, std::shared_ptr<ActorUpdate> > mappedUpdates;
	
	
	
	
	while(socket.receive(packet, address))
	{
		
		unsigned short temp;
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
				packet.reset();
				packet << localPort << std::string("create") << uniqueID << actorType;
				if(!socket.send(address, packet))
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
				
				
				

				packet >> creationResult;
				if(creationResult == "success")
				{
					std::cout << "actor has been created" << std::endl;
					packet >> actorID;
					controller.setActorID(actorID);
					state = State::RECEIVING_UPDATES;
					renderSystem.setMainActor(actorID);
					
				}
				
			}
			break;
		case State::RECEIVING_UPDATES:
			
			if(packetType == "update")
			{
				
				while(!packet.isEnd())
				{

					
					ComponentUpdate componentUpdate;
					packet >> componentUpdate;
				
					if(mappedUpdates.find(componentUpdate.actorID) == mappedUpdates.end())
					{
						mappedUpdates[componentUpdate.actorID] = std::make_shared<ActorUpdate>();
					}
					mappedUpdates[componentUpdate.actorID]->actorID = componentUpdate.actorID;

					

					if(componentUpdate.name == "move")
					{
						std::shared_ptr<MoveUpdate> moveUpdate = std::make_shared<MoveUpdate>();
						(ComponentUpdate&) *moveUpdate = componentUpdate;
					
						packet >> *moveUpdate;
					
						mappedUpdates[componentUpdate.actorID]->updates.push_back(moveUpdate);				
					}
					if(componentUpdate.name == "chat")
					{
						std::shared_ptr<ChatUpdate> chatUpdate = std::make_shared<ChatUpdate>();
						(ComponentUpdate&) *chatUpdate = componentUpdate;
						packet >> *chatUpdate;
						mappedUpdates[componentUpdate.actorID]->updates.push_back(chatUpdate);
					}
					if(componentUpdate.name == "delete")
					{
						std::shared_ptr<DeleteUpdate> deleteUpdate = std::make_shared<DeleteUpdate>();
						(ComponentUpdate&) *deleteUpdate = componentUpdate;
						packet >> *deleteUpdate;
						mappedUpdates[componentUpdate.actorID]->updates.push_back(deleteUpdate);
					}
					if(componentUpdate.name == "animation")
					{
						std::shared_ptr<AnimationUpdate> animationUpdate = std::make_shared<AnimationUpdate>();
						(ComponentUpdate&) *animationUpdate = componentUpdate;
						packet >> *animationUpdate;
						mappedUpdates[componentUpdate.actorID]->updates.push_back(animationUpdate);
					}

					

					Packet approvePacket;
				
					approvePacket << localPort << "approve" << uniqueID << componentUpdate.actorID << componentUpdate.name << componentUpdate.number;
					socket.send(address, approvePacket);
				}
			}
			else if(packetType == "approve")
			{
				std::string approveType;
				packet >> approveType;
				if(approveType == "move")
				{
					
					int number;
					packet >> number;
					controller.approve("move", number);
				}
				else if(approveType == "chat")
				{
					int number;
					packet >>number;
					controller.approve("chat", number);
				}
			}
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