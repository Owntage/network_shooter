#include "render_system.h"
#include <render_window.h>
#include <components/move_update.h>
#include <components/chat_update.h>
#include <components/animation_update.h>
#include <iostream>
#include <delete_update.h>

DrawableActor::DrawableActor(Console& console, std::map<std::string, sf::Texture>& textures) : isMain(false),
	rect(sf::Vector2f(1.0f, 1.0f)), console(console), lastMessagePrinted(-1), textures(textures)
{
	rect.setOrigin(0.5f, 0.5f);
	textureChangeTime = 0.0f;
	delay = 0.0f;
	currentAnimation = 0;
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
		if((*it)->name == "animation")
		{
			std::cout << "animation update got" << std::endl;
			AnimationUpdate& animationUpdate = static_cast<AnimationUpdate&>(*(*it));
			for(int i = 0; i < animationUpdate.states.size(); i++)
			{
				if(animationStates.find(animationUpdate.states[i].stateName) == animationStates.end())
				{
					
					animationStates[animationUpdate.states[i].stateName] = animationUpdate.states[i];
					std::cout << "state name: " << animationUpdate.states[i].stateName << std::endl;
					std::cout << "state delay: " << animationUpdate.states[i].delay << std::endl;
					for(int j = 0; j < animationUpdate.states[i].images.size(); j++)
					{
						std::cout << "image " << j + 1 << ": " << animationUpdate.states[i].images[j] << std::endl;
					}
				}
			}
			currentAnimationState = animationUpdate.animationState;
			delay = animationStates[currentAnimationState].delay;
		}
	}
}

void DrawableActor::draw()
{
	textureChangeTime += 1.0f / 60.0f;
	if(textureChangeTime > delay && delay != 0.0f)
	{
		textureChangeTime = 0.0f;
		currentAnimation = (currentAnimation + 1) % animationStates[currentAnimationState].images.size();
		std::string nextImage = animationStates[currentAnimationState].images[currentAnimation];
		if(textures.find(nextImage) == textures.end())
		{
			sf::Texture newTexture;
			newTexture.loadFromFile(nextImage);
			textures[nextImage] = newTexture;
		}
		
		rect.setTexture(&textures[nextImage]);
	}
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
			actors[(*it)->actorID] = std::make_shared<DrawableActor>(console, textures);
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