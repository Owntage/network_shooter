//SERVER
#include <iostream>
#include <string>
#include "server.h"
#include <game_logic.h>
#include <thread>
#include <chrono>
#include <network.h>
#include <string>

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
	
	auto time = chrono::system_clock::now();
	
	while(true)
	{
		this_thread::sleep_until(time);
		time = time + chrono::milliseconds(16);
		gameServer.receiveEvents();
		gameServer.sendUpdates();
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
		char buffer[128];
		IpAddress remoteAddress;
		Packet packet;
		if(socket.receive(packet, remoteAddress))
		{
			std::cout << "received packet" << std::endl;
			std::cout << "messages: " << std::endl;
			for(int i = 0; i < 3; i++)
			{
				float message;
				packet >> message;
				std::cout << "message " << i << ": " << message << std::endl;
			}
		}
		else
		{
			//std::cout << "receive returned false" << std::endl;
		}
	}

	UdpSocket::shutdownSockets();
	*/
	
	
	return 0;
}