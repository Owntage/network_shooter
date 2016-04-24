#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include "singleton.h"
#include <SFML/Graphics.hpp>

struct RenderWindow: Singleton<RenderWindow>
{
	RenderWindow() : window(sf::VideoMode(800, 600), "network_shooter")
	{
		window.setKeyRepeatEnabled(false);
	}
	sf::RenderWindow window;
};

#endif