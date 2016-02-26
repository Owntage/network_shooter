#include "controller.h"
#include <iostream>

void Controller::onEvent(sf::Event event)
{
	if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
	{
		
		bool isPressed = event.type == sf::Event::KeyPressed;
		std::cout << "button state: " << (bool) isPressed << std::endl;
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
			currentNumber++;
		}

	}
	
}

void Controller::approve(int systemID, int number)
{
	std::cout << "number received: " << number << std::endl;
	std::cout << "last number: " << approveNumbers[systemID] << std::endl;
	approveNumbers[systemID] = number;
}

int Controller::getCurrentNumber()
{
	return currentNumber;
}

void Controller::onEvent(WindowEvent& event)
{
	onEvent(event.sfmlEvent);
}

void Controller::setActorID(int actorID)
{
	moveEvent->actorID = actorID;
}

std::vector<std::shared_ptr<Event> > Controller::getGameEvents(int systemID)
{
	std::vector<std::shared_ptr<Event> > result;
	if(approveNumbers[systemID] < currentNumber)
	{
		result.push_back(moveEvent);
	}
	return result;
}