#include "render_system.h"
#include <render_window.h>
#include <components/move_update.h>
#include <components/chat_update.h>
#include <components/animation_update.h>
#include <components/tile_update.h>
#include <iostream>
#include <fstream>
#include <delete_update.h>


bool isFileExists(std::string filename)
{
	std::ifstream file(filename.c_str());
	return file.good();
}

DrawableActor::DrawableActor(Console& console, RenderSystem& renderSystem) : isMain(false),
	rect(sf::Vector2f(1.0f, 1.0f)), console(console), lastMessagePrinted(-1), renderSystem(renderSystem)
{
	rect.setOrigin(0.5f, 0.5f);
	textureChangeTime = 9999.0f;
	delay = 0.0f;
	isDrawing = false;
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
				}
			}
			currentAnimationState = animationUpdate.animationState;
			delay = animationStates[currentAnimationState].delay;
			isDrawing = true;
		}
		if((*it)->name == "tile")
		{
			//std::cout << "tile update got" << std::endl;
			TileUpdate& tileUpdate = static_cast<TileUpdate&>(*(*it));
			//std::cout << "image: " << tileUpdate.image << std::endl;
			//std::cout << "x: " << tileUpdate.x << std::endl;
			//std::cout << "y: " << tileUpdate.y << std::endl;
			if(renderSystem.imagesInTileset.find(tileUpdate.image) == renderSystem.imagesInTileset.end())
			{
				//it may not exist
				sf::Image image;
				if(!isFileExists(tileUpdate.image))
				{
					image.loadFromFile(DEFAULT_TEXTURE);
					if(renderSystem.imageLoadRequests.find(tileUpdate.image) == renderSystem.imageLoadRequests.end())
					{
						renderSystem.imageLoadRequests.insert(tileUpdate.image);
						renderSystem.imagesToLoad.push_back(tileUpdate.image);
					}
				}
				else
				{
					image.loadFromFile(tileUpdate.image);
				}
				
				int pos = renderSystem.imagesInTileset.size();
				renderSystem.imagesInTileset[tileUpdate.image] = pos;
				int tileset_x = pos % TILESET_WIDTH;
				int tileset_y = pos / TILESET_WIDTH;
				renderSystem.tileset.update(image, tileset_x * TILE_SIZE, tileset_y * TILE_SIZE);
			}
			
			float tileset_x = renderSystem.imagesInTileset[tileUpdate.image] % TILESET_WIDTH;
			float tileset_y = renderSystem.imagesInTileset[tileUpdate.image] / TILESET_WIDTH;

			tileset_x *= TILE_SIZE;
			tileset_y *= TILE_SIZE;

			//float f_width = TILESET_WIDTH;
			//float f_height = TILESET_HEIGHT;

			vertices[0].position = sf::Vector2f(tileUpdate.x - 0.5f, tileUpdate.y - 0.5f);
			vertices[1].position = sf::Vector2f(tileUpdate.x + 0.5f, tileUpdate.y - 0.5f);
			vertices[2].position = sf::Vector2f(tileUpdate.x + 0.5f, tileUpdate.y + 0.5f);
			vertices[3].position = sf::Vector2f(tileUpdate.x - 0.5f, tileUpdate.y + 0.5f);

			vertices[0].texCoords = sf::Vector2f(tileset_x, tileset_y);
			vertices[1].texCoords = sf::Vector2f(tileset_x + TILE_SIZE, tileset_y);
			vertices[2].texCoords = sf::Vector2f(tileset_x + TILE_SIZE, tileset_y + TILE_SIZE);
			vertices[3].texCoords = sf::Vector2f(tileset_x, tileset_y + TILE_SIZE);

			for(int i = 0; i < 4; i++)
			{
				renderSystem.tileVertices.append(vertices[i]);
			}
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
		if(renderSystem.textures.find(nextImage) == renderSystem.textures.end())
		{
			if(isFileExists(nextImage))
			{
				sf::Texture newTexture;
				newTexture.loadFromFile(nextImage);
				renderSystem.textures[nextImage] = newTexture;
			}
			else
			{
				if(renderSystem.textures.find(DEFAULT_TEXTURE) == renderSystem.textures.end())
				{
					sf::Texture defaultTexture;
					defaultTexture.loadFromFile(DEFAULT_TEXTURE);
					renderSystem.textures[DEFAULT_TEXTURE] = defaultTexture;
				}
				renderSystem.textures[nextImage] = renderSystem.textures[DEFAULT_TEXTURE];
				if(renderSystem.imageLoadRequests.find(nextImage) == renderSystem.imageLoadRequests.end())
				{
					renderSystem.imagesToLoad.push_back(nextImage);
				}
			}
		}
		
		rect.setTexture(&renderSystem.textures[nextImage]);
	}
	if(isDrawing)
	{
		if(isMain)
		{
			float view_x = renderSystem.gameView.getCenter().x;
			float view_y = renderSystem.gameView.getCenter().y;
			renderSystem.gameView.setCenter(rect.getPosition().x * 0.05 + view_x * 0.95, rect.getPosition().y * 0.05 + view_y * 0.95);
			RenderWindow::getInstance()->window.setView(renderSystem.gameView);
		}
		RenderWindow::getInstance()->window.draw(rect);
	}
	
}

RenderSystem::RenderSystem(Console& console) : gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f / 32.0f, 600.0f / 32.0f)), 
	mainActor(-1), console(console)
{
	tileset.create(TILESET_WIDTH * TILE_SIZE, TILESET_HEIGHT * TILE_SIZE);
	tileVertices.setPrimitiveType(sf::PrimitiveType::Quads);
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
			//std::cout << "render system created actor with id: " << (*it)->actorID << std::endl;
			actors[(*it)->actorID] = std::make_shared<DrawableActor>(console, *this);
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

	//sf::RenderStates renderStates(&tileset);
	RenderWindow::getInstance()->window.draw(tileVertices, &tileset);

	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		it->second->draw();
	}
}

void RenderSystem::setMainActor(int mainActor)
{
	this->mainActor = mainActor;
}

void RenderSystem::onImageLoaded(std::string image)
{
	//check later
	if(textures.find(image) != textures.end())
	{
		sf::Texture texture;
		texture.loadFromFile(image);
		textures[image] = texture;
	}
	if(imagesInTileset.find(image) != imagesInTileset.end())
	{
		sf::Image sfImage;
		sfImage.loadFromFile(image);
		int pos = imagesInTileset[image];
		int tileset_x = pos % TILESET_WIDTH;
		int tileset_y = pos / TILESET_WIDTH;
		tileset.update(sfImage, tileset_x * TILE_SIZE, tileset_y * TILE_SIZE);
	}
}