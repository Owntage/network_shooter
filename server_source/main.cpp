//SERVER
#include <iostream>
#include <string>
#include "server.h"
#include <game_logic.h>
//#include <thread>
#include <SFML/System.hpp>
//#include <chrono>
#include <network.h>
#include <string>
#include <components/world.h>

using namespace std;

#define RECEIVE_PORT 13337

uint32_t uniqueCounter = 0;



int main()
{
	
	UdpSocket::initializeSockets();
	
	cout << "shooter server started at port " << RECEIVE_PORT << endl;
	
	ActorFactory actorFactory("res/properties.xml");
	GameLogic gameLogic(actorFactory);
	GameServer gameServer(gameLogic, RECEIVE_PORT);
	
	//auto time = chrono::system_clock::now();
	
	while(true)
	{
		//this_thread::sleep_until(time);
		sf::sleep(sf::milliseconds(16));
		//time = time + chrono::milliseconds(16);
		gameServer.receiveEvents();
		gameServer.sendUpdates();
		World::getInstance()->update(60.0f);
		Event timerEvent("timer");
		gameLogic.onEvent(timerEvent);
	}
	
	UdpSocket::shutdownSockets();
	
	
	/*
	UdpSocket::initializeSockets();

	UdpSocket socket;
	socket.bind(13337);
	socket.setNonBlocking();

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		//std::cout << "waiting for message" << std::endl;
		IpAddress remoteAddress;
		Packet packet;
		if(socket.receive(packet, remoteAddress))
		{
			std::cout << "received packet" << std::endl;
			std::cout << "messages: " << std::endl;
			while(!packet.isEnd())
			{
				float temp;
				packet >> temp;
				std::cout << "message: " << temp << std::endl;
			}
		}
		
	}

	UdpSocket::shutdownSockets();
	*/
	
	
	return 0;
}