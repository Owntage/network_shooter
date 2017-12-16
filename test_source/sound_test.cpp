//
// Created by Матвей on 15.12.2017.
//

#include "test_suit.h"
#include <variant/variant_event.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;

void SoundTest::test()
{
	cout << "sound test started" << endl;

	SoundManager manager = SoundManager();
	manager.setListenerPosition(0, 0);

	int count = 0;

	manager.setObstaclePosition(0, 10, 5, 5);
	manager.setObstaclePosition(0, 30, 5, 5);

	sf::RenderWindow window(sf::VideoMode(500, 500), "SFML window");
	window.setFramerateLimit(60);

	sf::View view(sf::Vector2f(0, 100), sf::Vector2f(200, 200));
	window.setView(view);

	sf::RectangleShape wall;
	wall.setFillColor(sf::Color::White);
	wall.setSize(sf::Vector2f(5, 5));

	sf::RectangleShape listener;
	listener.setSize(sf::Vector2f(5, 5));
	listener.setFillColor(sf::Color::Green);
	listener.setPosition(0, 0);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				manager.playSound(0, 60, "res/gun-cocking-01.wav");

				count += 3;

				listener.setPosition(0, count);
				manager.setListenerPosition(0, count);
			}

			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.setActive();
		window.clear(sf::Color::Black);

		wall.setPosition(0, 10);
		window.draw(wall);
		wall.setPosition(0, 30);
		window.draw(wall);

		wall.setFillColor(sf::Color::Red);
		wall.setPosition(0, 60);
		window.draw(wall);
		wall.setFillColor(sf::Color::White);

		window.draw(listener);

		window.display();
	}
	cout << "sound test finished";
}