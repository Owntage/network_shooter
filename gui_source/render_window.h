#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include "singleton.h"
#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

struct RenderWindow: Singleton<RenderWindow>
{
	RenderWindow() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "network_shooter")
	{
		window.setKeyRepeatEnabled(false);
	}
	sf::RenderWindow window;
};

#endif