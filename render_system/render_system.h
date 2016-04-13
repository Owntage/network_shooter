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
	float textureChangeTime;
	float delay;
	int currentAnimation;
	
	std::string currentAnimationState;
	int lastMessagePrinted;
	sf::RectangleShape rect;
	std::map<std::string, AnimationState> animationStates;
	Console& console;
	RenderSystem& renderSystem;
};

struct RenderSystem
{
	RenderSystem(Console& console) : gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f / 32.0f, 600.0f / 32.0f)), 
		mainActor(-1), console(console) {}
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw();
	void setMainActor(int mainActor);
	void onImageLoaded(std::string image);
	std::vector<std::string> imagesToLoad;
private:
	std::map<int, std::shared_ptr<DrawableActor> > actors;
	std::map<std::string, sf::Texture> textures;
	std::set<int> deletedActors;
	std::set<std::string> imageLoadRequests;
	int mainActor;
	sf::View gameView;
	Console& console;
	friend class DrawableActor;
};

#endif