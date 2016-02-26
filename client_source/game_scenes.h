#ifndef GAME_SCENES_H
#define GAME_SCENES_H

#include <scenes.h>

struct MenuScene : Scene
{
	MenuScene();
	void onFrame();
	void onEvent(WindowEvent& event);
};

struct MainScene : Scene
{
	MainScene();
	void onFrame();
	void onEvent(WindowEvent& event);
};


#endif