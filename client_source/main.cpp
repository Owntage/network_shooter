//CLIENT

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <gui.h>
#include <render_window.h>
#include <actor_factory.h>
#include <game_logic.h>
#include <render_system.h>
#include <components/move_event.h>
#include "network_logic.h"
#include "controller.h"
using namespace std;

#define SEND_PORT 13337

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
	cout << "client is running!" << endl;
	
	
	//window init
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	
	//game logic init
	Controller controller;
	NetworkLogic networkLogic(sf::IpAddress("127.0.0.1"), 13337, "testActor", controller);
	RenderSystem renderSystem;

	
	
	
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
			controller.onEvent(event);
		}

		RenderWindow::getInstance()->window.clear();
		//rendering game_logic
		networkLogic.sendEvents();
		renderSystem.onUpdate(networkLogic.receiveUpdates());
		renderSystem.draw();

		//rendering gui...
		
		RenderWindow::getInstance()->window.display();
		
		
	}
	return 0;
}