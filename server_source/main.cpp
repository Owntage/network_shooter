//SERVER
#include <iostream>
#include <string>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "server.h"
#include <game_logic.h>
#include <render_system.h>
#include <render_window.h>
using namespace std;

#define RECEIVE_PORT 13337

sf::Uint32 uniqueCounter = 0;



int main()
{
	cout << "shooter server started at port " << RECEIVE_PORT << endl;
	
	ActorFactory actorFactory("res/properties.xml");
	GameLogic gameLogic(actorFactory);
	GameServer gameServer(gameLogic, RECEIVE_PORT);
	RenderSystem renderSystem;
	int renderSystemID = gameLogic.registerSystem();
		
	
	//window init
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	
	
	//main loop
	while(RenderWindow::getInstance()->window.isOpen())
	{
		//window events
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
		}
		
		
		RenderWindow::getInstance()->window.clear(sf::Color::Blue);
		
		//dealing with requests and events
		gameServer.receiveEvents();
		gameServer.sendUpdates();
		Event timerEvent("timer");
		gameLogic.onEvent(timerEvent);
		renderSystem.onUpdate(gameLogic.getUpdates(renderSystemID));
		renderSystem.draw();

		RenderWindow::getInstance()->window.display();
	}
	
	return 0;
}