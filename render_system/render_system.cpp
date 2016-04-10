#include "render_system.h"
#include <render_window.h>
#include <components/move_update.h>
#include <components/chat_update.h>
#include <iostream>
#include <delete_update.h>

DrawableActor::DrawableActor(Console& console) : isMain(false), rect(sf::Vector2f(1.0f, 1.0f)), console(console), lastMessagePrinted(-1)
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
		if((*it)->name == "chat" && isMain)
		{
			std::cout << "received chat message." << std::endl;
			ChatUpdate& chatUpdate = static_cast<ChatUpdate&> (*(*it));
			
			for(int i = 0; i < chatUpdate.messages.size(); i++)
			{
				if(i + chatUpdate.rangeBegin > lastMessagePrinted)
				{
					console.println(chatUpdate.messages[i]);
				}
			}
			lastMessagePrinted = chatUpdate.rangeEnd;
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
		if((*it)->actorID == -1)
		{
			
			for(auto update_it = (*it)->updates.begin(); update_it != (*it)->updates.end(); update_it++)
			{
				if((*update_it)->name == "delete")
				{
					
					DeleteUpdate& deleteUpdate = (DeleteUpdate&) *(*update_it);
					for(int i = 0; i < deleteUpdate.deletedActors.size(); i++)
					{
						int deletedActor = deleteUpdate.deletedActors[i];
						std::cout << "deleted actor: " << deletedActor << std::endl;
						if(actors.find(deletedActor) != actors.end())
						{
							deletedActors.insert(deletedActor);
							actors.erase(deletedActor);
						}
					}
				}
			}
			continue;
		}

		if(actors.find((*it)->actorID) == actors.end())
		{
			if(deletedActors.find((*it)->actorID) != deletedActors.end())
			{
				continue;
			}
			std::cout << "render system created actor with id: " << (*it)->actorID << std::endl;
			actors[(*it)->actorID] = std::make_shared<DrawableActor>(console);
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