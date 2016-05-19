#include "controller.h"
#include <math.h>
#include <iostream>
#include <components/chat_event.h>

#define PI 3.1415926536

Controller::Controller(Console& console, int tileSize, int screenWidth, int screenHeight) : 
	moveEvent(false, false, false, false, 0.0f, -1),
	console(console),
	tileSize(tileSize),
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	mouseX(0),
	mouseY(0)
{
	currentNumbers["move"] = 0;
	currentNumbers["chat"] = 0;
	approvedNumbers["move"] = -1;
	approvedNumbers["chat"] = 0;
	currentNumbers["mouse"] = 0;
	approvedNumbers["move"] = 0;

	currentNumbers["wheel_up"] = 0;
	approvedNumbers["wheel_up"] = 0;
	currentNumbers["wheel_down"] = 0;
	approvedNumbers["wheel_down"] = 0;
	isMessageSent = false;
	console.setInputCallback([this](std::string input)
	{
		
		currentNumbers["chat"]++;
		message = input;
	});
}

void Controller::onEvent(sf::Event event)
{
	if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
	{
		
		bool isPressed = event.type == sf::Event::KeyPressed;
		//std::cout << "button state: " << (bool) isPressed << std::endl;
		MoveEvent lastEvent = moveEvent;
		switch(event.key.code)
		{
		case sf::Keyboard::W:
			moveEvent.up = isPressed;
			break;
		case sf::Keyboard::A:
			moveEvent.left = isPressed;
			break;
		case sf::Keyboard::D:
			moveEvent.right = isPressed;
			break;
		case sf::Keyboard::S:
			moveEvent.down = isPressed;
			break;
		}
		if(!(lastEvent == moveEvent))
		{
			currentNumbers["move"]++;
		}

	}
	if(event.type == sf::Event::MouseMoved)
	{
		
		mouseX = event.mouseMove.x - screenWidth / 2;
		mouseY = event.mouseMove.y - screenHeight / 2;
		mouseX /= tileSize;
		mouseY /= tileSize;
		
		//mouseX += cameraDeltaX;
		//mouseY += cameraDeltaY;

		
	}
	if(event.type == sf::Event::MouseButtonPressed)
	{
		currentNumbers["mouse"]++;
		shootingEventName = "shooting_begin";
	}
	if(event.type == sf::Event::MouseButtonReleased)
	{
		currentNumbers["mouse"]++;
		shootingEventName = "shooting_end";
	}
	if(event.type == sf::Event::MouseWheelMoved)
	{
		int delta = event.mouseWheel.delta;
		if(delta > 0)
		{
			currentNumbers["wheel_up"]++;
		}
		else
		{
			currentNumbers["wheel_down"]++;
		}
	}
	
}

void Controller::approve(std::string type, int number)
{
	//std::cout << "number received for " << type << ": " << number << std::endl;
	//std::cout << "last number: " << approvedNumbers[type] << std::endl;
	if(approvedNumbers.find(type) != approvedNumbers.end())
	{
		approvedNumbers[type] = number;
	}
}



void Controller::onEvent(WindowEvent& event)
{
	onEvent(event.sfmlEvent);
}

void Controller::setActorID(int actorID)
{
	moveEvent.actorID = actorID;
}

std::vector<std::shared_ptr<Event> > Controller::getGameEvents()
{
	std::vector<std::shared_ptr<Event> > result;
	
	if(approvedNumbers["move"] < currentNumbers["move"])
	{
		moveEvent.number = currentNumbers["move"];
		result.push_back(std::make_shared<MoveEvent>(moveEvent));
	}
	if(approvedNumbers["chat"] < currentNumbers["chat"])
	{
		//isMessageSent = true;
		//std::cout << "pushed message: " << message << std::endl;
		std::shared_ptr<ChatEvent> chatEvent = std::make_shared<ChatEvent>(message, moveEvent.actorID);
		chatEvent->number = currentNumbers["chat"];
		result.push_back(chatEvent);
	}
	if(approvedNumbers["mouse"] < currentNumbers["mouse"])
	{
		auto event = std::make_shared<Event>(shootingEventName, false, moveEvent.actorID);
		event->number = currentNumbers["mouse"];
		result.push_back(event);
		//result.push_back(std::make_shared<Event>("shoot", false, moveEvent.actorID));
	}
	if(approvedNumbers["wheel_up"] < currentNumbers["wheel_up"])
	{
		auto event = std::make_shared<Event>("next_weapon", false, moveEvent.actorID);
		event->number = currentNumbers["wheel_up"];
		result.push_back(event);
	}
	if(approvedNumbers["wheel_down"] < currentNumbers["wheel_down"])
	{
		auto event = std::make_shared<Event>("prev_weapon", false, moveEvent.actorID);
		event->number = currentNumbers["wheel_down"];
		result.push_back(event);
	}
	return result;
}

void Controller::updateFromRenderSystem(RenderSystem& renderSystem)
{
	cameraX = renderSystem.getCameraX();
	cameraY = renderSystem.getCameraY();
	playerX = renderSystem.getPlayerX();
	playerY = renderSystem.getPlayerY();

	float cameraDeltaX = playerX - cameraX;
	float cameraDeltaY = playerY - cameraY;

	mouseX -= cameraDeltaX;
	mouseY -= cameraDeltaY;

	float newAngle;
	if(mouseX < 0)
	{
		newAngle = atan(mouseY / mouseX) + PI;
	}
	else
	{
		if(mouseY < 0)
		{
			newAngle = atan(mouseY / mouseX) + PI * 2;
		}
		else
		{
			newAngle = atan(mouseY / mouseX);
		}
	}
	if(moveEvent.angle != newAngle)
	{
		currentNumbers["move"]++;
		newAngle /= PI;
		newAngle *= 180.0f;
		moveEvent.angle = newAngle;
	}
	
	mouseX += cameraDeltaX;
	mouseY += cameraDeltaY;

}
