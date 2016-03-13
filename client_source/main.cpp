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
	std::string connectAddress;
	cout << "enter address: ";
	cin >> connectAddress;
	cout << "client is running!" << endl;
	
	
	//window init
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	
	//game logic init
	GuiManager guiManager(800, 600);
	Console console(-WINDOW_WIDTH / 2 + CONSOLE_WIDTH / 2, WINDOW_HEIGHT / 2 - CONSOLE_HEIGHT / 2, CONSOLE_WIDTH, CONSOLE_HEIGHT, guiManager);
	Controller controller(console);
	RenderSystem renderSystem(console);
	NetworkLogic networkLogic(sf::IpAddress(connectAddress), 13337, "testActor", controller, renderSystem);
	

	
	
	
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
			guiManager.onEvent(event);
		}

		RenderWindow::getInstance()->window.clear();
		//rendering game_logic
		networkLogic.sendEvents();
		renderSystem.onUpdate(networkLogic.receiveUpdates());
		renderSystem.draw();

		//rendering gui...
		guiManager.draw();

		RenderWindow::getInstance()->window.display();
		
		
	}
	return 0;
}