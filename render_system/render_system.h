#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <actor_update.h>
#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include <gui.h>
#include <set>
#include <components/animation_component.h>



#define DEFAULT_TEXTURE "res/graphic/default.png"

struct RenderSystem;

struct DrawableActor
{
	DrawableActor(Console& console, RenderSystem& renderSystem);
	void onUpdate(ActorUpdate& actorUpdate);
	void draw();
	void setMain(bool isMain);
private:
	bool isMain;
	bool isDrawing;
	bool animationStateChanged;
	float serverTime;

	float deltaTime;
	float positionX;
	float positionY;
	float speedX;
	float speedY;
	std::vector<LayerState> animationLayerStates;
	std::vector<float> layerTime;
	std::vector<int> layerImageIndex;
	int lastMessagePrinted;
	sf::RectangleShape rect;
	sf::Vertex vertices[4];
	std::map<std::string, AnimationState> animationStates;
	Console& console;
	RenderSystem& renderSystem;
};

#define TILE_SIZE 32
#define TILESET_WIDTH 16
#define TILESET_HEIGHT 16

struct RenderSystem
{
	RenderSystem(Console& console);
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw();
	void setMainActor(int mainActor);
	void onImageLoaded(std::string image);
	float getCameraX();
	float getCameraY();
	float getPlayerX();
	float getPlayerY();
	std::vector<std::string> imagesToLoad;
private:
	
	std::map<int, std::shared_ptr<DrawableActor> > actors;
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, int> imagesInTileset;
	std::set<int> deletedActors;
	std::set<std::string> imageLoadRequests;
	int mainActor;
	float playerX;
	float playerY;
	float cameraX;
	float cameraY;
	sf::View gameView;
	sf::VertexArray tileVertices;
	sf::Texture tileset;
	Console& console;
	friend class DrawableActor;
};

#endif