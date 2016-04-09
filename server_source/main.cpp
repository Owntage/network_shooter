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

sf::Uint32 uniqueCounter = 0;



int main()
{
	/*
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
	*/
	UdpSocket::initializeSockets();

	UdpSocket socket;
	socket.bind(13337);

	while(true)
	{
		std::cout << "waiting for message" << std::endl;
		char buffer[128];
		IpAddress remoteAddress;
		Packet packet;
		socket.receive(packet, remoteAddress);
		std::cout << "received packet" << std::endl;
		std::cout << "messages: " << std::endl;
		for(int i = 0; i < 4; i++)
		{
			std::string message;
			packet >> message;
			std::cout << "message " << i << ": " << message << std::endl;
		}
	}

	UdpSocket::shutdownSockets();
	
	return 0;
}