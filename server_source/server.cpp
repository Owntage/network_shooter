#include "server.h"
#include <components/move_component.h>
#include <string>
#include <iostream>
#include <components/move_event.h>
#include <components/chat_component.h>
#include <components/animation_update.h>
#include <components/tile_update.h>
#include <components/coord_event.h>
#include <components/string_event.h>
#include <components/render_component.h>
#include <components/weapon_component.h>
#include <components/hp_component.h>
#include <delete_update.h>
#include "level_loader.h"


GameServer::GameServer(GameLogic& gameLogic, unsigned short port): uniqueCounter(0), gameLogic(gameLogic), serverView(gameLogic, gameLogic.registerSystem())
{
	socket.bind(port);
	socket.setNonBlocking();
	//serverViewSystemID = gameLogic.registerSystem();
	/*
	for(int i = 0; i < 200; i++)
	{
		for(int j = 0; j < 200; j++)
		{
			int testTile = gameLogic.createActor("tileActor");
			gameLogic.onEvent(CoordEvent("set_coords", testTile, 5.0f + i - 100, 1.0f + j - 100));
			gameLogic.onEvent(StringEvent("set_image", testTile, "res/graphic/concr1.png"));
		}
	}
	*/
	LevelLoader levelLoader(gameLogic);
	levelLoader.loadLevel("res", "level1.tmx");
}

bool GameServer::ensureClient(IpAddress& address, int uniqueID)
{
	bool result = true;
	if(clients.find(uniqueID) == clients.end())
	{
		result = false;
	}
	else
	{
		if(!(clients[uniqueID].address == address))
		{
			result = false;
		}
	}
	if(!result)
	{
		Packet tempPacket;
		tempPacket << "incorrect_packet";
		socket.send(address, tempPacket);
	}
	return result;
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
			if(ensureClient(remoteAddress, uniqueID))
			{
				refreshTimeout(uniqueID);
			}
			
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
			//packet >> uniqueID >> actorID >> updateName >> updateNumber;
			packet >> uniqueID;
			if(ensureClient(remoteAddress, uniqueID))
			{
				packet >> actorID >> updateName >> updateNumber;
				if(clients.find(uniqueID) != clients.end())
				{
					gameLogic.approve(actorID, updateName, clients[uniqueID].systemID, updateNumber);
				}	
			}
				
		}
		else if(type == "image_size")
		{
			std::string imageName;
			packet >> imageName;
			if(images.find(imageName) == images.end())
			{
				images[imageName] = std::make_shared<File>(imageName);
			}
			packet.reset();
			packet << "image_size" << imageName << images[imageName]->size;
			socket.send(remoteAddress, packet);
		}
		else if(type == "image_data")
		{
			std::string imageName;
			FileMask mask;
			packet >> imageName >> mask;
			packet.reset();
			packet << "image_data" << imageName;
			images[imageName]->printToPacket(packet, mask);
			socket.send(remoteAddress, packet);
		}
		
	}
	
}

void GameServer::receiveGameEvent(Packet& packet, IpAddress& address)
{
	int uniqueID;
	packet >> uniqueID;
	if(ensureClient(address, uniqueID))
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
			bool isSpecialEvent = false;
			if(event.name == "move")
			{
				MoveEvent moveEvent;
				(Event&) moveEvent = event;
				packet >> moveEvent;
				gameLogic.onEvent(moveEvent);
				isSpecialEvent = true;
			}
			if(event.name == "chat")
			{
				ChatEvent chatEvent;
				(Event&) chatEvent = event;
				packet >> chatEvent;
				gameLogic.onEvent(chatEvent);
				isSpecialEvent = true;
			}
			/*
			if(event.name == "shoot")
			{
				gameLogic.onEvent(event);

			}
			*/
			if(!isSpecialEvent)
			{
				gameLogic.onEvent(event);
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

	//getting updates for serverView
	serverView.onUpdate(gameLogic.getUpdates(serverView.systemID));

	//sending updates
	for(auto client_it = clients.begin(); client_it != clients.end(); client_it++)
	{
		
		
		Packet packet;
		packet << "update";
		
		int clientActorID = client_it->second.gameLogicID;
		ActorCoords mainActorCoords = serverView.getCoords(clientActorID);
		
		bool packed = false;
		auto updates = gameLogic.getUpdates(client_it->second.systemID, mainActorCoords.x, mainActorCoords.y, client_it->second.radiusMultiplier * UPDATE_RADIUS);
		for(auto actor_it = updates.begin(); actor_it != updates.end(); actor_it++)
		{

			for(auto component_it = (*actor_it)->updates.begin(); component_it != (*actor_it)->updates.end(); component_it++)
			{
				
				packet.setCursor();

				packet << *(*component_it);

				if((*component_it)->name == "move") packet << (MoveUpdate&) *(*component_it);
				if((*component_it)->name == "chat") packet << (ChatUpdate&) *(*component_it);
				if((*component_it)->name == "delete") packet << (DeleteUpdate&) *(*component_it);
				if((*component_it)->name == "animation") packet << (AnimationUpdate&) *(*component_it);
				if((*component_it)->name == "tile") packet << (TileUpdate&) *(*component_it);
				if((*component_it)->name == "render") packet << (RenderUpdate&) *(*component_it);
				if((*component_it)->name == "weapon") packet << (WeaponUpdate&) *(*component_it);
				if((*component_it)->name == "hp") packet << (HpUpdate&) *(*component_it);

				if(packet.isPacked())
				{
					packet.revertToCursor();
					client_it->second.radiusMultiplier = 1.0f;
					packed = true;
				}
			}
		}

		//if there are not enough updates in this range, update radius is increased
		//if there is too much updates to put in one packet, radius is dropped back to default
		if(!packed)
		{
			if(client_it->second.radiusMultiplier < 100)
			{
				client_it->second.radiusMultiplier += 1.0f;
			}
		}
		
		
		socket.send(client_it->second.address, packet);
		
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
	int uniqueID = rand();
	while(clients.find(uniqueID) != clients.end())
	{
		uniqueID = rand();
	}
	packet << "id" << uniqueID;
	//uniqueCounter++;
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

void GameServer::ServerView::onUpdate(std::vector<std::shared_ptr<ActorUpdate> > update)
{
	for(auto it = update.begin(); it != update.end(); it++)
	{
		
		if((*it)->actorID == -1)
		{
			for(auto component_it = (*it)->updates.begin(); component_it != (*it)->updates.end(); component_it++)
			{
				gameLogic.approve(-1, (*component_it)->name, systemID, (*component_it)->number);
				if((*component_it)->name == "delete")
				{
					DeleteUpdate& deleteUpdate = (DeleteUpdate&) *(*component_it);
					for(int i = 0; i < deleteUpdate.deletedActors.size(); i++)
					{
						if(actors.find(deleteUpdate.deletedActors[i]) != actors.end())
						{
							deletedActors.insert(deleteUpdate.deletedActors[i]);
							actors.erase(deleteUpdate.deletedActors[i]);
						}
					}
				}
			}
			continue;
		}
		if(actors.find((*it)->actorID) == actors.end())
		{
			if(deletedActors.find((*it)->actorID) != deletedActors.end())
			{
				continue;
			}
			actors[(*it)->actorID] = std::make_shared<ActorCoords>();
			actors[(*it)->actorID]->actorExists = true;
			actors[(*it)->actorID]->hasCoords = false;
			
		}	
		for(auto component_it = (*it)->updates.begin(); component_it != (*it)->updates.end(); component_it++)
		{
			gameLogic.approve((*component_it)->actorID, (*component_it)->name, systemID, (*component_it)->number);	
			if((*component_it)->name == "move")
			{
				MoveUpdate& moveUpdate = (MoveUpdate&) *(*component_it);
				actors[(*it)->actorID]->hasCoords = true;
				actors[(*it)->actorID]->x = moveUpdate.x;
				actors[(*it)->actorID]->y = moveUpdate.y;
				(*it)->actor->x = moveUpdate.x;
				(*it)->actor->y = moveUpdate.y;
				(*it)->actor->hasCoords = true;
				
			}
			if((*component_it)->name == "tile")
			{
				TileUpdate& tileUpdate = (TileUpdate&) *(*component_it);
				actors[(*it)->actorID]->hasCoords = true;
				actors[(*it)->actorID]->x = tileUpdate.x;
				actors[(*it)->actorID]->y = tileUpdate.y;
				(*it)->actor->x = tileUpdate.x;
				(*it)->actor->y = tileUpdate.y;
				(*it)->actor->hasCoords = true;
			}
		}
	}
}

GameServer::ActorCoords GameServer::ServerView::getCoords(int actorID)
{
	if(actors.find(actorID) == actors.end())
	{
		ActorCoords result;
		result.actorExists = false;
		return result;
	}
	return *actors.find(actorID)->second;
}