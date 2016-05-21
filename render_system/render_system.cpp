#include "render_system.h"
#include <render_window.h>
#include <components/move_update.h>
#include <components/chat_update.h>
#include <components/animation_update.h>
#include <components/deathmatch_component.h>
#include <components/tile_update.h>
#include <iostream>
#include <fstream>
#include <delete_update.h>
#include <math.h>
#include <vector>


LightManager::LightManager(float screenWidth, float screenHeight, float tileWidth) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	tileWidth(tileWidth),
	counter(0)
{
	shader.loadFromFile(LIGHT_VERTEX_SHADER, LIGHT_FRAGMENT_SHADER);
	multiplyShader.loadFromFile(MULTIPLY_FRAGMENT_SHADER, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / 4, screenHeight / 4);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int LightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	LightSource lightSource;
	sf::Vertex vertex;
	vertex.color = color;
	vertex.color.a = intensity;
	vertex.texCoords = pos;
	vertex.position = sf::Vector2f(-screenWidth / 2, -screenHeight / 2);
	lightSource.vertices[0] = vertex;
	vertex.position = sf::Vector2f(screenWidth / 2, -screenHeight / 2);
	lightSource.vertices[1] = vertex;
	vertex.position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
	lightSource.vertices[2] = vertex;
	vertex.position = sf::Vector2f(-screenWidth / 2, screenHeight / 2);
	lightSource.vertices[3] = vertex;

	verticesMap[counter] = lightSource;
	
	return counter++;
}

void LightManager::draw(sf::RenderTarget& renderTarget)
{
	sf::RenderStates renderStates;
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendAdd;
	sf::Vector2f center = renderTarget.getView().getCenter();
	center.x *= tileWidth;
	center.y *= tileWidth;
	shader.setParameter("offset", center);
	
	std::vector<sf::Vertex> vertices;
	for(auto it = verticesMap.begin(); it != verticesMap.end(); it++)
	{
		for(int i = 0; i < 4; i++)
		{
			vertices.push_back(it->second.vertices[i]);
		}
	}

	sf::View lightView;
	lightView.setCenter(sf::Vector2f(0, 0));
	lightView.setSize(screenWidth, screenHeight);
	renderTexture.setView(lightView);
	renderTexture.clear();
	renderTexture.draw(&vertices[0], vertices.size(), sf::Quads, renderStates);
	renderTexture.display();
	shape.setTexture(&renderTexture.getTexture());
	shape.setPosition(renderTarget.getView().getCenter());

	sf::RenderStates multiplyRenderStates;

	multiplyRenderStates.blendMode = sf::BlendAdd;
	multiplyShader.setParameter("multiplier", 5.0f * 0.2f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setParameter("multiplier", 5.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
}

void LightManager::setPosition(int lightSourceID, sf::Vector2f pos)
{
	verticesMap[lightSourceID].vertices[0].texCoords = pos;
	verticesMap[lightSourceID].vertices[1].texCoords = pos;
	verticesMap[lightSourceID].vertices[2].texCoords = pos;
	verticesMap[lightSourceID].vertices[3].texCoords = pos;
}

void LightManager::removeLightSource(int lightSourceID)
{
	verticesMap.erase(lightSourceID);
}


bool isFileExists(std::string filename)
{
	std::ifstream file(filename.c_str());
	return file.good();
}

DrawableActor::DrawableActor(Console& console, RenderSystem& renderSystem, std::shared_ptr<LightManager> lightManager) : 
	isMain(false),
	rect(sf::Vector2f(1.0f, 1.0f)), 
	console(console), 
	lastMessagePrinted(-1), 
	renderSystem(renderSystem),
	deltaTime(0.0f),
	positionX(0.0f),
	positionY(0.0f),
	speedX(0.0f),
	speedY(0.0f),
	lightManager(lightManager),
	hasLightSource(false)
{
	rect.setOrigin(0.5f, 0.5f);
	isDrawing = false;
	animationStateChanged = false;
	serverTime = -1.0f;
	
}

DrawableActor::~DrawableActor()
{
	if(hasLightSource)
	{
		lightManager->removeLightSource(lightSourceID);
	}
}

void DrawableActor::setMain(bool isMain)
{
	this->isMain = isMain;
}

void DrawableActor::onUpdate(ActorUpdate& update)
{
	//std::cout << "onUpdate started" << std::endl;
	for(auto it = update.updates.begin(); it != update.updates.end(); it++)
	{
		if((*it)->name == "move")
		{
			MoveUpdate& moveUpdate = static_cast<MoveUpdate&> (*(*it));
			positionX = moveUpdate.x;
			positionY = moveUpdate.y;
			speedX = moveUpdate.speedX;
			speedY = moveUpdate.speedY;
			serverTime = moveUpdate.time;
			deltaTime = 0;
		}
		if((*it)->name == "render")
		{
			RenderUpdate& renderUpdate = static_cast<RenderUpdate&> (*(*it));
			renderData = renderUpdate.renderData;
		}
		if((*it)->name == "weapon")
		{
			
			WeaponUpdate& weaponUpdate = static_cast<WeaponUpdate&> (*(*it));
			if(isMain)
			{
				renderSystem.gameGuiManager.setWeaponUpdate(weaponUpdate);
			}
		}
		if((*it)->name == "deathmatch")
		{
			std::cout << "got deathmatch update" << std::endl;

			DeathmatchUpdate dmUpdate = static_cast<DeathmatchUpdate&>(*(*it));

			std::cout << "actors in update: " << dmUpdate.actors.size() << std::endl;
		}
		if((*it)->name == "hp")
		{
			HpUpdate& hpUpdate = static_cast<HpUpdate&> (*(*it));
			if(isMain)
			{
				renderSystem.gameGuiManager.setHpUpdate(hpUpdate);
			}
		}
		if((*it)->name == "chat" && isMain)
		{
			//std::cout << "received chat message." << std::endl;
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
			//std::cout << "animation update got" << std::endl;
			AnimationUpdate& animationUpdate = static_cast<AnimationUpdate&>(*(*it));
			//std::cout << "state: " << animationUpdate.animationState << std::endl;
			for(int i = 0; i < animationUpdate.states.size(); i++)
			{
				if(animationStates.find(animationUpdate.states[i].stateName) == animationStates.end())
				{
					animationStates[animationUpdate.states[i].stateName] = animationUpdate.states[i];
				}
			}
			
			if(animationLayerStates.size() != animationUpdate.currentLayerStates.size())
			{
				animationStateChanged = true;
			}
			else
			{
				animationStateChanged = false;
				for(int i = 0; i < animationLayerStates.size(); i++)
				{
					if(animationLayerStates[i] != animationUpdate.currentLayerStates[i])
					{
						animationStateChanged = true;
						break;
					}
				}
			}
			
			animationLayerStates = animationUpdate.currentLayerStates;
			layerTime.resize(animationLayerStates.size(), 99999.0f);
			layerImageIndex.resize(animationLayerStates.size(), 0);
			isDrawing = true;
		}
		if((*it)->name == "tile")
		{
			TileUpdate& tileUpdate = static_cast<TileUpdate&>(*(*it));
			if(renderSystem.imagesInTileset.find(tileUpdate.image) == renderSystem.imagesInTileset.end())
			{
				sf::Image image;
				if(!isFileExists(tileUpdate.image))
				{
					//std::cout << "loading image in tile update" << std::endl;
					image.loadFromFile(DEFAULT_TEXTURE);
					if(renderSystem.imageLoadRequests.find(tileUpdate.image) == renderSystem.imageLoadRequests.end())
					{
						renderSystem.imageLoadRequests.insert(tileUpdate.image);
						renderSystem.imagesToLoad.push_back(tileUpdate.image);
					}
				}
				else
				{
					//std::cout << "loading default image in tile update" << std::endl;
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
	//std::cout << "onUpdate finished" << std::endl;
}



void DrawableActor::draw()
{
	
	rect.setPosition(positionX + speedX * deltaTime, positionY + speedY * deltaTime);
	deltaTime += 1.0f / 60.0f;

	

	if(isDrawing)
	{
		for(int i = 0; i < layerTime.size(); i++)
		{
			if(!animationLayerStates[i].isDrawing)
			{
				continue;
			}
			layerTime[i] += 1.0f / 60.0f;
			
			
			if(layerTime[i] > animationStates[animationLayerStates[i].state].delay) //no check if animation state is changed
			{
				animationStateChanged = false;
				layerTime[i] = 0;
				
				layerImageIndex[i] = (layerImageIndex[i] + 1) % animationStates[animationLayerStates[i].state].images.size();
				
			}
			
			layerImageIndex[i] = layerImageIndex[i] % animationStates[animationLayerStates[i].state].images.size();
			std::string nextImage = animationStates[animationLayerStates[i].state].images[layerImageIndex[i]];
			if(renderSystem.textures.find(nextImage) == renderSystem.textures.end())
			{
				if(isFileExists(nextImage) && nextImage != "")
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
			rect.setRotation(animationLayerStates[i].angle);
			if(isMain)
			{
				float view_x = renderSystem.gameView.getCenter().x;
				float view_y = renderSystem.gameView.getCenter().y;
				renderSystem.gameView.setCenter(rect.getPosition().x * 0.05 + view_x * 0.95, rect.getPosition().y * 0.05 + view_y * 0.95);
				renderSystem.cameraX = renderSystem.gameView.getCenter().x;
				renderSystem.cameraY = renderSystem.gameView.getCenter().y;
				renderSystem.playerX = rect.getPosition().x;
				renderSystem.playerY = rect.getPosition().y;
				
			}
			RenderWindow::getInstance()->window.setView(renderSystem.gameView);
			if(renderData.spriteIsVisible)
			{
				rect.setScale(sf::Vector2f(renderData.spriteScale, renderData.spriteScale));
				RenderWindow::getInstance()->window.draw(rect);

			}
			

		}
	}
	
	if(renderData.lightIsVisible)
	{
		if(!hasLightSource)
		{
			lightSourceID = lightManager->addLightSource(sf::Vector2f(0, 0), 
				sf::Color(renderData.lightR, renderData.lightG, renderData.lightB), 
				renderData.lightIntensity);
			hasLightSource = true;
		}
		lightManager->setPosition(lightSourceID, sf::Vector2f((positionX + speedX * deltaTime) * TILE_SIZE, (positionY + speedY * deltaTime) * TILE_SIZE));
	}
	
}

RenderSystem::RenderSystem(Console& console, GuiManager& guiManager, float screenWidth, float screenHeight) : 
	gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(screenWidth / TILE_SIZE, screenHeight / TILE_SIZE)), 
	mainActor(-1), console(console), 
	gameGuiManager(screenWidth, screenHeight, guiManager)
{
	tileset.create(TILESET_WIDTH * TILE_SIZE, TILESET_HEIGHT * TILE_SIZE);
	tileVertices.setPrimitiveType(sf::PrimitiveType::Quads);
	lightManager = std::make_shared<LightManager>(screenWidth, screenHeight, TILE_SIZE);

	WeaponData weaponData;
	weaponData.timeSinceReload = 0;
	weaponData.timeSinceShot = 0;
	weaponData.shotsMade = 0;

	WeaponDef weaponDef;
	weaponDef.weaponTexture = NO_WEAPON_TEXTURE;
	weaponDef.bulletsPerHolder = 10;
	weaponDef.reloadTime = 4.0f;
	weaponDef.period = 1.0f;

	WeaponUpdate weaponUpdate(weaponDef, weaponData, WeaponUpdate::WeaponState::CHANGE);
	gameGuiManager.setWeaponUpdate(weaponUpdate);
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
						//std::cout << "deleted actor: " << deletedActor << std::endl;
						deletedActors.insert(deletedActor);
						if(actors.find(deletedActor) != actors.end())
						{
							
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
			actors[(*it)->actorID] = std::make_shared<DrawableActor>(console, *this, lightManager);
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
	lightManager->draw(RenderWindow::getInstance()->window);

	gameGuiManager.onTimer();
	gameGuiManager.draw(RenderWindow::getInstance()->window);

}

void RenderSystem::setMainActor(int mainActor)
{
	this->mainActor = mainActor;
}

void RenderSystem::onKey(int key, bool isReleased)
{
	gameGuiManager.onKey(key, isReleased);
}

void RenderSystem::onImageLoaded(std::string image)
{
	//check later
	if(textures.find(image) != textures.end())
	{
		sf::Texture texture;
		//std::cout << "loading image in onImageLoaded" << std::endl;
		texture.loadFromFile(image);
		textures[image] = texture;
	}
	if(imagesInTileset.find(image) != imagesInTileset.end())
	{
		sf::Image sfImage;
		//std::cout << "loading image for tileset in onImageLoaded" << std::endl;
		sfImage.loadFromFile(image);
		int pos = imagesInTileset[image];
		int tileset_x = pos % TILESET_WIDTH;
		int tileset_y = pos / TILESET_WIDTH;
		tileset.update(sfImage, tileset_x * TILE_SIZE, tileset_y * TILE_SIZE);
	}
}

float RenderSystem::getCameraX()
{
	return cameraX;
}

float RenderSystem::getCameraY()
{
	return cameraY;
}

float RenderSystem::getPlayerX()
{
	return playerX;
}

float RenderSystem::getPlayerY()
{
	return playerY;
}