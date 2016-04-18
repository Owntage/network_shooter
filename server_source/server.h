#ifndef SERVER_H
#define SERVER_H

//#include <SFML/Network.hpp>
#include <map>
#include <set>
#include <string>
#include <network.h>
#include <game_logic.h>



struct ClientData
{
	IpAddress address;
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
	//contract:
	//returns true, if client exists and address matches
	//returns false and sends "incorrect_packet", if something is not matching
	bool ensureClient(IpAddress& remoteAddress, int uniqueID);
	
	void sendUniqueID(IpAddress& remoteAddress);
	void createActor(IpAddress& remoteAddress, int uniqueID, std::string actorID);
	void destroyActor(int uniqueID);
	void refreshTimeout(int uniqueID); 
	void sendSuccessfulCreation(int uniqueID);
	void approve(int uniqueID, int actorID, std::string component, int number);
	void receiveGameEvent(Packet& packet, IpAddress& address);
	//sf::UdpSocket sendingSocket;
	UdpSocket socket;
	int uniqueCounter;
	GameLogic& gameLogic;
	std::map<int, ClientData> clients;
	std::map<std::string, std::shared_ptr<File> > images;

	struct ActorCoords
	{
		bool actorExists;
		bool hasCoords;
		float x;
		float y;
	};

	struct ServerView
	{
		ServerView(GameLogic& gameLogic, int systemID) : gameLogic(gameLogic), systemID(systemID)
		{
			std::cout << "serverView systemID: " << systemID << std::endl;
		}
		void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
		ActorCoords getCoords(int actorID);
		int systemID;
	private:
		std::map<int, std::shared_ptr<ActorCoords> > actors;
		std::set<int> deletedActors;
		
		GameLogic& gameLogic;
	};

	int serverViewSystemID;
	ServerView serverView;
};

#endif