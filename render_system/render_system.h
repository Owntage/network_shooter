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
#include <components/render_component.h>

#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"

struct LightManager
{
	LightManager(float screenWidth, float screenHeight);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
private:
	sf::VertexArray vertices;
	sf::Shader shader;
	sf::RectangleShape shape;
	sf::RenderTexture renderTexture;
	float screenWidth;
	float screenHeight;
};


#define DEFAULT_TEXTURE "res/graphic/default.png"

struct RenderSystem;

struct DrawableActor
{
	DrawableActor(Console& console, RenderSystem& renderSystem, std::shared_ptr<LightManager> lightManager);
	~DrawableActor();
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
	std::map<std::string, AnimationState> animationStates;

	sf::RectangleShape rect;
	sf::Vertex vertices[4];
	
	Console& console;
	int lastMessagePrinted;
	RenderSystem& renderSystem;

	std::shared_ptr<LightManager> lightManager;
	bool hasLightSource;
	int lightSourceID;

	RenderData renderData;

};

#define TILE_SIZE 32
#define TILESET_WIDTH 16
#define TILESET_HEIGHT 16

struct RenderSystem
{
	RenderSystem(Console& console, float screenWidth, float screenHeight);
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
	std::shared_ptr<LightManager> lightManager;
	friend class DrawableActor;
};

#endif