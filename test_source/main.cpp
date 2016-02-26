#include <SFML/Graphics.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
using namespace std;

struct WindowEvent
{
	WindowEvent(sf::Event& sfmlEvent) : sfmlEvent(sfmlEvent) {} 
	sf::Event& sfmlEvent;
};

struct testScene2 : Scene
{
	testScene2(sf::Color color) : color(color) {}
	void onFrame()
	{
		RenderWindow::getInstance()->window.clear(color);
		RenderWindow::getInstance()->window.display();
	}
	
	void onEvent(WindowEvent& event)
	{
		sf::Event& sfmlEvent = event.sfmlEvent;
		if(sfmlEvent.type == sf::Event::KeyPressed && sfmlEvent.key.code == sf::Keyboard::Escape)
		{
			SceneManager::getInstance()->closeScene();
		}
	}
	
private:
	sf::Color color;
};

struct testScene : Scene
{
	testScene()
	{
	}
	void onFrame()
	{
		RenderWindow::getInstance()->window.clear(sf::Color(16, 0, 16));
		RenderWindow::getInstance()->window.display();
	}
	void onEvent(WindowEvent& event)
	{
		sf::Event& sfmlEvent = event.sfmlEvent;
		if(sfmlEvent.type == sf::Event::KeyPressed)
		{
			switch(sfmlEvent.key.code)
			{
				case sf::Keyboard::Escape:
					SceneManager::getInstance()->closeScene();
					break;
				case sf::Keyboard::N:
					SceneManager::getInstance()->startScene(make_shared<testScene2>(sf::Color(0, 16, 16)));
					break;
			}
		}
	}
};

int main()
{
	
	SceneManager::getInstance()->startScene(make_shared<testScene>());
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	
	while(RenderWindow::getInstance()->window.isOpen())
	{
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
			WindowEvent windowEvent(event);
			SceneManager::getInstance()->onEvent(windowEvent);
		}
		
		if(SceneManager::getInstance()->isEmpty())
		{
			RenderWindow::getInstance()->window.close();
		}
		
		SceneManager::getInstance()->onFrame();
		
		
	}
	return 0;
}