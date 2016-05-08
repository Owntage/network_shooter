#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <chrono>
#include <sstream>
#include <render_system.h>
#include <network.h>
using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100


#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"
#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"




int main()
{
	LightManager lightManager(800, 600, 1);
	for(int i = 0; i < 50; i++)
	{
		int x = (rand() % 100) - 50;
		int y = (rand() % 100) - 50;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		int intensity = rand() % 10 + 1;
		lightManager.addLightSource(sf::Vector2f(x, y), sf::Color(r, g, b), intensity);
	}

	int specialLightSource = lightManager.addLightSource(sf::Vector2f(0, 0), sf::Color::Cyan, 20);
	float phase = 0;

	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(100.0, 0.0), sf::Vector2f(800, 600)));
	RenderWindow::getInstance()->window.setFramerateLimit(60);

	int frameCount = 0;
	bool deleteFlag = false;

	while(RenderWindow::getInstance()->window.isOpen())
	{
		phase += 3.14 / 60.0;
		frameCount++;
		lightManager.setPosition(specialLightSource, sf::Vector2f(cos(phase) * 400.0f, sin(phase) * 300.0f));
		
		if(frameCount > 500 && !deleteFlag)
		{
			lightManager.removeLightSource(specialLightSource);
			deleteFlag = true;
		}


		RenderWindow::getInstance()->window.clear();
		lightManager.draw(RenderWindow::getInstance()->window);
		RenderWindow::getInstance()->window.display();
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
		}
	}
	

	return 0;
}