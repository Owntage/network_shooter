#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <actor_update.h>
#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>

struct DrawableActor
{
	DrawableActor();
	void onUpdate(ActorUpdate& actorUpdate);
	void draw();
	void setMain(bool isMain);
private:
	bool isMain;
	sf::RectangleShape rect;
};

struct RenderSystem
{
	RenderSystem() : gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f / 32.0f, 600.0f / 32.0f)), mainActor(-1) {}
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw();
	void setMainActor(int mainActor);
private:
	std::map<int, std::shared_ptr<DrawableActor> > actors;
	int mainActor;
	sf::View gameView;
};

#endif