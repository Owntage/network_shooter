#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <map>
#include <string>
#include <game_logic.h>



struct ClientData
{
	sf::IpAddress address;
	unsigned short port;
	int gameLogicID;
	int systemID;
	float time;
	std::map<std::string, int> eventNumbers;
};

struct GameServer
{
	GameServer(GameLogic& gameLogic, unsigned short port);
	void receiveEvents();
	void sendUpdates();
private:
	void sendUniqueID(sf::IpAddress& remoteAddress, unsigned short remotePort);
	void createActor(sf::IpAddress& remoteAddress, unsigned short remotePort, int uniqueID, std::string actorID);
	void destroyActor(int uniqueID);
	void refreshTimeout(int uniqueID); 
	void sendSuccessfulCreation(int uniqueID);
	void approve(int uniqueID, int actorID, std::string component, int number);
	void receiveGameEvent(sf::Packet& packet, sf::IpAddress& address, unsigned short port);
	
	//sf::UdpSocket sendingSocket;
	sf::UdpSocket sendingSocket;
	sf::UdpSocket receivingSocket;
	int systemID;
	int uniqueCounter;
	GameLogic& gameLogic;
	std::map<int, ClientData> clients;
};

#endif