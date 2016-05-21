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
#include <components/weapon_component.h>
#include <math.h>
using namespace std;


#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100


int main()
{
	
	//GameGuiManager gameGuiManager(800, 600);

	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(100.0, 0.0), sf::Vector2f(800, 600)));
	RenderWindow::getInstance()->window.setFramerateLimit(60);

	WeaponData weaponData;
	weaponData.timeSinceReload = 0;
	weaponData.timeSinceShot = 0;
	weaponData.shotsMade = 0;

	WeaponDef weaponDef;
	weaponDef.bulletsPerHolder = 10;
	weaponDef.reloadTime = 4.0f;
	weaponDef.period = 1.0f;
	
	WeaponUpdate weaponUpdate(weaponDef, weaponData, WeaponUpdate::WeaponState::CHANGE);
	//gameGuiManager.setWeaponUpdate(weaponUpdate);
	
	while(RenderWindow::getInstance()->window.isOpen())
	{
		RenderWindow::getInstance()->window.clear();

		//gameGuiManager.onTimer();
		//gameGuiManager.draw(RenderWindow::getInstance()->window);

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