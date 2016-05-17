#include "network_logic.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <components/move_update.h>
#include <components/chat_component.h>
#include <components/animation_update.h>
#include <components/tile_update.h>
#include <components/render_component.h>
#include <components/weapon_component.h>
#include <components/hp_component.h>
#include <delete_update.h>
#include "game_scenes.h"


NetworkLogic::NetworkLogic(IpAddress address, std::string actorType, Controller& controller, RenderSystem& renderSystem) : 
	address(address), 
	state(State::GETTING_UNIQUE_ID), 
	actorType(actorType), controller(controller), 
	renderSystem(renderSystem),
	scenePushed(false)
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
			std::cout << "creation request sent successfully" << std::endl;
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
			
			packet.reset();
			packet << localPort << "event" << uniqueID << (*(*it));
			if((*it)->name == "move")
			{
				packet << (MoveEvent&) (*(*it));
			}
			if((*it)->name == "chat")
			{
				packet << (ChatEvent&) (*(*it));
			}
			socket.send(address, packet);
		}

		//send request for images;
		while(renderSystem.imagesToLoad.size() > 0)
		{
			downloadingFiles[renderSystem.imagesToLoad.back()];
			std::cout << "downloading image: " << renderSystem.imagesToLoad.back() << std::endl;
			renderSystem.imagesToLoad.pop_back();
		}

		for(auto it = downloadingFiles.begin(); it != downloadingFiles.end(); )
		{
			if(it->second.isDownloaded())
			{
				downloadingFiles.erase(it++);
			}
			else
			{
				it++;
			}
		}

		for(auto it = downloadingFiles.begin(); it != downloadingFiles.end(); it++)
		{
			packet.reset();
			if(it->second.sizeIsKnown)
			{
				packet << localPort << "image_data" << it->first << it->second.mask;
			}
			else
			{
				packet << localPort << "image_size" << it->first;
			}
			socket.send(address, packet);
		}
		break;
	}
}

template<typename UPDATE_T>
void addUpdate(std::shared_ptr<ActorUpdate> actorUpdate, ComponentUpdate& componentUpdate, Packet& packet, bool shouldBeWritten)
{

	std::shared_ptr<UPDATE_T> newUpdate = std::make_shared<UPDATE_T>();
	(ComponentUpdate&) *newUpdate = componentUpdate;
	packet >> *newUpdate;
	if(shouldBeWritten)
	{
		actorUpdate->updates.push_back(newUpdate);
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
		if(packetType == "incorrect_packet")
		{
			//std::cout << "server is restarted. popping the scene from scene stack" << std::endl;
			
			if(!scenePushed)
			{
				SceneManager::getInstance()->closeScene();
				SceneManager::getInstance()->startScene(std::make_shared<ConnectScene>(800, 600, true));
				scenePushed = true;
			}
			
		}
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
					std::cout << "creation request sent successfully" << std::endl;
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

					bool shouldBeWritten = true;

					if(componentUpdate.name == "move")
						addUpdate<MoveUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "chat")
						addUpdate<ChatUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "delete")
						addUpdate<DeleteUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "animation")
						addUpdate<AnimationUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "tile")
						addUpdate<TileUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "render")
						addUpdate<RenderUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "weapon")
						addUpdate<WeaponUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					if(componentUpdate.name == "hp")
						addUpdate<HpUpdate>(mappedUpdates[componentUpdate.actorID], componentUpdate, packet, shouldBeWritten);
					

					

					Packet approvePacket;
				
					approvePacket << localPort << "approve" << uniqueID << componentUpdate.actorID << componentUpdate.name << componentUpdate.number;
					socket.send(address, approvePacket);
				}
			}
			else if(packetType == "approve")
			{
				std::string approveType;
				int number;
				packet >> approveType >> number;
				controller.approve(approveType, number);
				

			}
			else if(packetType == "image_size")
			{
				std::string imageName;
				int size;
				packet >> imageName >> size;
				std::cout << "size of an image " << imageName << " :" << size << std::endl;
				if(downloadingFiles.find(imageName) != downloadingFiles.end())
				{
					downloadingFiles[imageName].setSize(size);
				}
			}
			else if(packetType == "image_data")
			{
				
					std::string imageName;
					packet >> imageName;
					if(downloadingFiles.find(imageName) != downloadingFiles.end())
					{
						packet >> downloadingFiles[imageName];
						if(downloadingFiles[imageName].isDownloaded())
						{
							downloadingFiles[imageName].saveToFile(imageName);
							renderSystem.onImageLoaded(imageName);
						}
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