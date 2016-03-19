#include "controller.h"
#include <iostream>
#include <components/chat_event.h>

Controller::Controller(Console& console) : moveEvent(std::make_shared<MoveEvent>(false, false, false, false, -1)), console(console)
{
	currentNumbers["move"] = 0;
	currentNumbers["chat"] = 0;
	approvedNumbers["move"] = 0;
	approvedNumbers["chat"] = 0;
	isMessageSent = false;
	console.setInputCallback([this](std::string input)
	{
		std::cout << "hello from console callback! " << std::endl;
		std::cout << "currentNumber " << currentNumbers["chat"] << std::endl;
		std::cout << "last approved number: " << approvedNumbers["chat"] << std::endl;
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
		MoveEvent lastEvent = *moveEvent;
		switch(event.key.code)
		{
		case sf::Keyboard::W:
			moveEvent->up = isPressed;
			break;
		case sf::Keyboard::A:
			moveEvent->left = isPressed;
			break;
		case sf::Keyboard::D:
			moveEvent->right = isPressed;
			break;
		case sf::Keyboard::S:
			moveEvent->down = isPressed;
			break;
		}
		if(!(lastEvent == *moveEvent))
		{
			currentNumbers["move"]++;
		}

	}
	
}

void Controller::approve(std::string type, int number)
{
	std::cout << "number received for " << type << ": " << number << std::endl;
	std::cout << "last number: " << approvedNumbers[type] << std::endl;
	approvedNumbers[type] = number;
}



void Controller::onEvent(WindowEvent& event)
{
	onEvent(event.sfmlEvent);
}

void Controller::setActorID(int actorID)
{
	moveEvent->actorID = actorID;
}

std::vector<std::shared_ptr<Event> > Controller::getGameEvents()
{
	std::vector<std::shared_ptr<Event> > result;
	
	if(approvedNumbers["move"] < currentNumbers["move"])
	{
		moveEvent->number = currentNumbers["move"];
		result.push_back(moveEvent);
	}
	if(approvedNumbers["chat"] < currentNumbers["chat"])
	{
		//isMessageSent = true;
		std::cout << "pushed message: " << message << std::endl;
		std::shared_ptr<ChatEvent> chatEvent = std::make_shared<ChatEvent>(message, moveEvent->actorID);
		chatEvent->number = currentNumbers["chat"];
		result.push_back(chatEvent);
	}
	return result;
}

