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



int main(int argc, char** argv)
{
	
	UdpSocket::initializeSockets();
	
	cout << "shooter server started at port " << RECEIVE_PORT << endl;
	
	ActorFactory actorFactory("res/properties.xml");
	GameLogic gameLogic(actorFactory);
	GameServer gameServer(gameLogic, RECEIVE_PORT);
	
	//auto time = chrono::system_clock::now();

	sf::Clock clock;
	
	while(true)
	{
		//this_thread::sleep_until(time);
		//sf::sleep(sf::milliseconds(16));
		//time = time + chrono::milliseconds(16);
		gameServer.receiveEvents();
		gameServer.sendUpdates();
		World::getInstance()->update(60.0f);
		Event timerEvent("timer");
		gameLogic.onEvent(timerEvent);
		sf::sleep(sf::milliseconds(16) - clock.getElapsedTime());
		clock.restart();
	}
	
	UdpSocket::shutdownSockets();

	return 0;
}