#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include "singleton.h"
#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct RenderWindow: Singleton<RenderWindow>
{
	RenderWindow() : window(sf::VideoMode::getDesktopMode(), "network_shooter")
	{
		window.setKeyRepeatEnabled(false);
	}
	sf::RenderWindow window;

	float getWidth()
	{
		return window.getSize().x;
	}

	float getHeight()
	{
		return window.getSize().y;
	}
};

#endif