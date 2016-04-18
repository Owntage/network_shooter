#ifndef GAME_SCENES_H
#define GAME_SCENES_H

#include <scenes.h>
#include "gui.h"
#include "controller.h"
#include "network.h"
#include <render_system.h>
#include "network_logic.h"



struct MenuScene : Scene
{
	MenuScene(float screenWidth, float screenHeight);
	void onFrame();
	void onEvent(WindowEvent& event);
private:
	float screenWidth;
	float screenHeight;
	GuiManager guiManager;
	Button startButton;
	Button optionsButton;
	WindowPanel startPanel;
	WindowPanel optionsPanel;
	NinePatchSprite playLabelSprite;
	NinePatchSprite optionsLabelSprite;
	NinePatchSprite normalButtonSprite;
	NinePatchSprite hoveredButtonSprite;
	NinePatchSprite pressedButtonSprite;
};

#define SERVER_PORT 13337

struct ConnectScene : Scene
{
	ConnectScene(float screenWidth, float screenHeight);
	void onFrame();
	void onEvent(WindowEvent& event);
private:
	void onPingSuccessful();
	float screenWidth;
	float screenHeight;
	bool sendingConnection;
	int packetCount;
	std::string serverAddress;
	NinePatchSprite normalButtonSprite;
	NinePatchSprite hoveredButtonSprite;
	NinePatchSprite pressedButtonSprite;
	NinePatchSprite backSprite;
	GuiManager guiManager;
	WindowPanel panel;
	Button backButton;
	TextView text;
	TextView alertText;
	InputField inputField;
	UdpSocket socket;
};

struct MainScene : Scene
{
	MainScene(float windowWidth, float windowHeight, std::string address);
	void onFrame();
	void onEvent(WindowEvent& event);
private:
	GuiManager guiManager;
	Console console;
	Controller controller;
	RenderSystem renderSystem;
	NetworkLogic networkLogic;
};


#endif