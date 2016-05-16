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
#include <render_system.h>

struct WindowEvent
{
	WindowEvent(sf::Event& sfmlEvent) : sfmlEvent(sfmlEvent) {} 
	sf::Event& sfmlEvent;
};

#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 200

struct Controller
{
	Controller(Console& console, int tileSize, int screenWidth, int screenHeight);
	void onEvent(sf::Event event);
	void onEvent(WindowEvent& event);
	void setActorID(int actorID);
	void approve(std::string type, int number);
	void updateFromRenderSystem(RenderSystem& renderSystem);
	std::vector<std::shared_ptr<Event> > getGameEvents();
private:
	int tileSize;
	int screenWidth;
	int screenHeight;
	float cameraX;
	float cameraY;
	float playerX;
	float playerY;

	float mouseX;
	float mouseY;
	float cameraDeltaX;
	float cameraDeltaY;

	bool isMessageSent;
	std::string message;
	std::string shootingEventName;
	MoveEvent moveEvent;
	std::map<int, MoveEvent> mappedEvents;
	std::map<std::string, int> approvedNumbers;
	std::map<std::string, int> currentNumbers;
	Console& console;
};

#endif