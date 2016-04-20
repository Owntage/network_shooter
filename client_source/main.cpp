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
#include "game_scenes.h"
using namespace std;







#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
	UdpSocket::initializeSockets();

	/*
	std::string connectAddress;
	cout << "enter address: ";
	cin >> connectAddress;
	cout << "client is running!" << endl;
	*/
	
	
	//window init
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	
	//game logic init
	//GuiManager guiManager(800, 600);
	//Console console(-WINDOW_WIDTH / 2 + CONSOLE_WIDTH / 2, WINDOW_HEIGHT / 2 - CONSOLE_HEIGHT / 2, CONSOLE_WIDTH, CONSOLE_HEIGHT, guiManager);
	//Controller controller(console, TILE_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT);
	//RenderSystem renderSystem(console);
	SceneManager::getInstance()->startScene(std::make_shared<MenuScene>(WINDOW_WIDTH, WINDOW_HEIGHT));
	while(RenderWindow::getInstance()->window.isOpen())
	{
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
				break;
			}
			if(!SceneManager::getInstance()->isEmpty())
			{
				SceneManager::getInstance()->onEvent(WindowEvent(event));
			}
		}
		if(SceneManager::getInstance()->isEmpty())
		{
			break;
		}
		SceneManager::getInstance()->onFrame();
	}
	

	UdpSocket::shutdownSockets();
	return 0;
}