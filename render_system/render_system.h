#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <actor_update.h>
#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include <gui.h>
#include <set>

struct DrawableActor
{
	DrawableActor(Console& console);
	void onUpdate(ActorUpdate& actorUpdate);
	void draw();
	void setMain(bool isMain);
private:
	bool isMain;
	int lastMessagePrinted;
	sf::RectangleShape rect;
	Console& console;
};

struct RenderSystem
{
	RenderSystem(Console& console) : gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f / 32.0f, 600.0f / 32.0f)), 
		mainActor(-1), console(console) {}
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw();
	void setMainActor(int mainActor);
private:
	std::map<int, std::shared_ptr<DrawableActor> > actors;
	std::set<int> deletedActors;
	int mainActor;
	
	sf::View gameView;
	Console& console;
};

#endif