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
#include <components/weapon_component.h>
#include <components/hp_component.h>
#include <components/deathmatch_component.h>

#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"

#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"

#define HP_TEXTURE "res/gui/hp.png"
#define NO_WEAPON_TEXTURE "res/gui/no_weapon.png"

struct GameGuiManager
{
	GameGuiManager(float screenWidth, float screenHeight, GuiManager& guiManager) :
		screenWidth(screenWidth),
		screenHeight(screenHeight),
		view(sf::Vector2f(0, 0), sf::Vector2f(screenWidth, screenHeight)),
		shootBlink(0.0f),
		damageBlink(0.0f),
		outputConsole(0, -100, 200, 300, guiManager)
	{
		hpUpdate.currentHp = 1;
		hpUpdate.maxHp = 1;
		outputConsole.setVisible(false);
	}
	void setWeaponUpdate(WeaponUpdate& weaponUpdate);
	void setHpUpdate(HpUpdate& hpUpdate);
	void setDeathmatchUpdate(DeathmatchUpdate& deathmatchUpdate);
	void draw(sf::RenderTarget& renderTarget);
	void onTimer();
	void onKey(int key, bool isReleased);

private:

	float screenWidth;
	float screenHeight;
	float shootBlink;
	float damageBlink;
	sf::View view;
	sf::RectangleShape shape;
	std::map<std::string, sf::Texture> textures;
	WeaponUpdate weaponUpdate;
	HpUpdate hpUpdate;
	sf::Texture& getTexture(std::string name);
	OutputConsole outputConsole;
};

struct LightManager
{
	LightManager(float screenWidth, float screenHeight, float tileWidth);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	struct LightData
	{
		sf::Vector2f position;
		sf::Vector3f color;
		float intensity;
		LightData() {}
		LightData(sf::Vector2f position, sf::Vector3f color, float intensity) :
				position(position),
				color(color),
				intensity(intensity)
		{}
	};
	std::map<int, int> idToShaderIndex; //can be replaced with bimap
	std::map<int, int> shaderIndexToId;
	std::map<int, LightData> idToData;
	int shaderArraySize;
	int counter;
	sf::Shader shader;
	sf::Shader multiplyShader;
	sf::RectangleShape shape;
	sf::RenderTexture renderTexture;
	float screenWidth;
	float screenHeight;
	float tileWidth;
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
	RenderSystem(Console& console, GuiManager& guiManager, float screenWidth, float screenHeight);
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw();
	void setMainActor(int mainActor);
	void onImageLoaded(std::string image);
	float getCameraX();
	float getCameraY();
	float getPlayerX();
	float getPlayerY();
	void onKey(int key, bool isReleased);
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
	GameGuiManager gameGuiManager;
	friend class DrawableActor;
};

#endif