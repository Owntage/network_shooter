#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <network.h>
using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100



int main()
{
	
	/*
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(), sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)));
	GuiManager guiManager(WINDOW_WIDTH, WINDOW_HEIGHT);
	Console console(-WINDOW_WIDTH / 2 + CONSOLE_WIDTH / 2, WINDOW_HEIGHT / 2 - CONSOLE_HEIGHT / 2, CONSOLE_WIDTH, CONSOLE_HEIGHT, guiManager);
	console.setInputCallback([&console](std::string input)
	{
		console.println(input);
	});
	
	auto fpsTime = chrono::system_clock::now() + chrono::seconds(1);
	int fpsCount = 0;

	while(RenderWindow::getInstance()->window.isOpen())
	{
		fpsCount++;
		if(chrono::system_clock::now() > fpsTime)
		{
			//std::cout << "fps: " << fpsCount << std::endl;
			
			fpsTime = fpsTime + chrono::seconds(1);
			fpsCount = 0;
		}


		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					RenderWindow::getInstance()->window.close();
					break;
				default:
					guiManager.onEvent(event);
					break;
			}
		}
		RenderWindow::getInstance()->window.clear();
		guiManager.draw();
		RenderWindow::getInstance()->window.display();	
	}
	*/
	UdpSocket::initializeSockets();

	IpAddress serverAddress("localhost", 13337);
	UdpSocket socket;
	socket.send(serverAddress, "hello", 5);
	
	UdpSocket::shutdownSockets();

	return 0;
}