//
// Created by Матвей on 15.12.2017.
//

#include "test_suit.h"
#include <variant/variant_event.h>
#include <SFML/Window.hpp>

void SoundTest::test() {
    SoundManager manager = SoundManager();
    manager.setListenerPosition(0, 0);

    int count = 0;

    manager.setObstaclePosition(0, 10, 5, 5);
    manager.setObstaclePosition(0, 30, 5, 5);

    sf::Window window(sf::VideoMode(100, 100), "SFML window");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed) {
                manager.playSound(0, 60, "gun-cocking-01.wav");

                count += 20;

                manager.setListenerPosition(0, count);
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.setActive();
        window.display();
    }
}