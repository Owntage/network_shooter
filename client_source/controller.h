#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <scenes.h>
#include <render_window.h>
#include <event.h>
#include <vector>
#include <memory>
#include <components/move_event.h>
#include <SFML/Graphics.hpp>
#include <map>

struct WindowEvent
{
	WindowEvent(sf::Event& sfmlEvent) : sfmlEvent(sfmlEvent) {} 
	sf::Event& sfmlEvent;
};

struct Controller
{
	Controller() : moveEvent(std::make_shared<MoveEvent>(false, false, false, false, -1)), currentNumber(0) {}
	void onEvent(sf::Event event);
	void onEvent(WindowEvent& event);
	void setActorID(int actorID);
	void approve(int systemID, int number);
	std::vector<std::shared_ptr<Event> > getGameEvents(int systemID);
	int getCurrentNumber();
private:
	std::shared_ptr<MoveEvent> moveEvent;
	std::map<int, MoveEvent> mappedEvents;
	std::map<int, int> approveNumbers;
	int currentNumber;
};

#endif