#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <scenes.h>
#include <render_window.h>
#include <event.h>
#include <vector>
#include <string>
#include <memory>
#include <components/move_event.h>
#include <SFML/Graphics.hpp>
#include <gui.h>
#include <map>

struct WindowEvent
{
	WindowEvent(sf::Event& sfmlEvent) : sfmlEvent(sfmlEvent) {} 
	sf::Event& sfmlEvent;
};

#define CONSOLE_WIDTH 200
#define CONSOLE_HEIGHT 100

struct Controller
{
	Controller(Console& console);
	void onEvent(sf::Event event);
	void onEvent(WindowEvent& event);
	void setActorID(int actorID);
	void approve(std::string type, int number);
	std::vector<std::shared_ptr<Event> > getGameEvents();
private:
	bool isMessageSent;
	std::string message;
	std::shared_ptr<MoveEvent> moveEvent;
	std::map<int, MoveEvent> mappedEvents;
	std::map<std::string, int> approvedNumbers;
	std::map<std::string, int> currentNumbers;
	Console& console;
};

#endif