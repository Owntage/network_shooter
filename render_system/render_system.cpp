#include "render_system.h"
#include <render_window.h>
#include <components/move_update.h>
#include <iostream>

DrawableActor::DrawableActor() : isMain(false), rect(sf::Vector2f(1.0f, 1.0f))
{
	rect.setOrigin(0.5f, 0.5f);
}

void DrawableActor::setMain(bool isMain)
{
	this->isMain = isMain;
}

void DrawableActor::onUpdate(ActorUpdate& update)
{
	for(auto it = update.updates.begin(); it != update.updates.end(); it++)
	{
		if((*it)->name == "move")
		{
			MoveUpdate& moveUpdate = static_cast<MoveUpdate&> (*(*it));
			rect.setPosition(moveUpdate.x, moveUpdate.y);
		}
	}
}

void DrawableActor::draw()
{
	RenderWindow::getInstance()->window.draw(rect);
}

void RenderSystem::onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates)
{
	for(auto it = updates.begin(); it != updates.end(); it++)
	{
		if(actors.find((*it)->actorID) == actors.end())
		{
			std::cout << "render system created actor with id: " << (*it)->actorID << std::endl;
			actors[(*it)->actorID] = std::make_shared<DrawableActor>();
			if((*it)->actorID == mainActor)
			{
				actors[(*it)->actorID]->setMain(true);
			}
		}

		actors[(*it)->actorID]->onUpdate(*(*it));
		
	}
}

void RenderSystem::draw()
{
	RenderWindow::getInstance()->window.setView(gameView);
	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		it->second->draw();
	}
}

void RenderSystem::setMainActor(int mainActor)
{
	this->mainActor = mainActor;
}