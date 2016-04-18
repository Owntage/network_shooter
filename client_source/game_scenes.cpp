#include "game_scenes.h"
#include "render_window.h"
#include <iostream>

MenuScene::MenuScene(float screenWidth, float screenHeight) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	guiManager(screenWidth, screenHeight),
	normalButtonSprite("res/gui/sci_fi/button_normal.png", true),
	hoveredButtonSprite("res/gui/sci_fi/button_hovered.png", true),
	pressedButtonSprite("res/gui/sci_fi/button_pressed.png", true),
	playLabelSprite("res/gui/sci_fi/play_label.png", false),
	optionsLabelSprite("res/gui/sci_fi/options_label.png", false),
	startButton(0, 0, 200, 50),
	optionsButton(0, 57, 200, 50),
	startPanel(0, 0, 64, 16, playLabelSprite),
	optionsPanel(0, 0, 64, 16, optionsLabelSprite)
{
	startButton.setNormalSprite(normalButtonSprite);
	startButton.setHoveredSprite(hoveredButtonSprite);
	startButton.setPressedSprite(pressedButtonSprite);
	startButton.setOnReleaseCallback([this]()
	{
		SceneManager::getInstance()->startScene(std::make_shared<ConnectScene>(this->screenWidth, this->screenHeight, false));
	});
	
	optionsButton.setNormalSprite(normalButtonSprite);
	optionsButton.setHoveredSprite(hoveredButtonSprite);
	optionsButton.setPressedSprite(pressedButtonSprite);
	
	guiManager.addElement(startButton);
	guiManager.addElement(optionsButton);
	guiManager.addElement(startPanel);
	guiManager.addElement(optionsPanel);
	
	optionsPanel.makeChildOf(optionsButton);
	startPanel.makeChildOf(startButton);
	//playLabel.makeChildOf(startButton);
	//guiManager.addElement(playLabel);
}

void MenuScene::onFrame()
{
	RenderWindow::getInstance()->window.clear(sf::Color(0, 0, 32));
	guiManager.draw();
	RenderWindow::getInstance()->window.display();
}

void MenuScene::onEvent(WindowEvent& event)
{
	switch(event.sfmlEvent.type)
	{
		case sf::Event::MouseButtonPressed:
			guiManager.onMouseClick(event.sfmlEvent.mouseButton.x - screenWidth / 2, event.sfmlEvent.mouseButton.y - screenHeight / 2, false);
			break;
		case sf::Event::MouseButtonReleased:
			guiManager.onMouseClick(event.sfmlEvent.mouseButton.x - screenWidth / 2, event.sfmlEvent.mouseButton.y - screenHeight / 2, true);
			break;
		case sf::Event::MouseMoved:
			guiManager.onMouseMove(event.sfmlEvent.mouseMove.x - screenWidth / 2, event.sfmlEvent.mouseMove.y - screenHeight / 2);
			break;
	}
}

ConnectScene::ConnectScene(float screenWidth, float screenHeight, bool restarted) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	sendingConnection(false),
	guiManager(screenWidth, screenHeight),
	normalButtonSprite("res/gui/sci_fi/button_normal.png", true),
	hoveredButtonSprite("res/gui/sci_fi/button_hovered.png", true),
	pressedButtonSprite("res/gui/sci_fi/button_pressed.png", true),
	backSprite("res/gui/sci_fi/back_label.png", false),
	panel(0, 0, 400, 200, normalButtonSprite),
	backButton(0, 133, 200, 50),
	text(0, -48, 350, 32),
	alertText(0, 48, 350, 32),
	inputField(0, 0, 350, 32, "res/gui/console/input_background.png", "res/gui/console/input_hovered_background.png",
		"res/gui/console/input_active_background.png")

{
	backButton.setNormalSprite(normalButtonSprite);
	backButton.setHoveredSprite(hoveredButtonSprite);
	backButton.setPressedSprite(pressedButtonSprite);
	backButton.setOnReleaseCallback([]()
	{
		SceneManager::getInstance()->closeScene();
	});

	text.setCharacterSize(32);
	text.setText("enter server address:");
	text.setColor(sf::Color::Black);

	alertText.setCharacterSize(20);
	if(!restarted)
	{
		alertText.setText("it's free. just press enter");
	}
	else
	{
		alertText.setText("server has restarted. ");
	}
	alertText.setColor(sf::Color(64,64,64));

	inputField.setInputCallback([this](std::string input)
	{
		this->serverAddress = input;
		this->sendingConnection = true;
		this->alertText.setColor(sf::Color(0, 64, 0));
		this->alertText.setText("connecting...");
		this->packetCount = 0;
	});


	guiManager.addElement(backButton);
	guiManager.addElement(text);
	guiManager.addElement(inputField);
	guiManager.addElement(panel);
	guiManager.addElement(alertText);

	backButton.makeChildOf(panel);
	text.makeChildOf(panel);
	inputField.makeChildOf(panel);
	alertText.makeChildOf(panel);
}

void ConnectScene::onPingSuccessful()
{
	//alertText.setText("ping successful");
	SceneManager::getInstance()->closeScene();
	SceneManager::getInstance()->startScene(std::make_shared<MainScene>(screenWidth, screenHeight, serverAddress));
}

void ConnectScene::onFrame()
{
	if(sendingConnection)
	{
		packetCount++;
		if(packetCount > 100)
		{
			alertText.setColor(sf::Color(128, 0, 0));
			alertText.setText("server is not responding. try again");
			sendingConnection = false;
		}
		else
		{
			IpAddress address(serverAddress, 13337);
			if(address.isCorrect())
			{
				Packet packet;
				packet << (unsigned short) socket.getLocalPort() << "id";
				socket.send(address, packet);
				IpAddress receiveAddress;
				if(socket.receive(packet, receiveAddress))
				{
					if(receiveAddress == address)
					{
						onPingSuccessful();
						sendingConnection = false;
					}
				}
			}
			else
			{
				alertText.setColor(sf::Color(128, 0, 0));
				alertText.setText("incorrect address");
				sendingConnection = false;
			}
			
		}
	}

	RenderWindow::getInstance()->window.clear(sf::Color(0, 0, 32));
	guiManager.draw();
	RenderWindow::getInstance()->window.display();
}

void ConnectScene::onEvent(WindowEvent& event)
{
	switch(event.sfmlEvent.type)
	{
		case sf::Event::MouseButtonPressed:
			guiManager.onMouseClick(event.sfmlEvent.mouseButton.x - screenWidth / 2, event.sfmlEvent.mouseButton.y - screenHeight / 2, false);
			break;
		case sf::Event::MouseButtonReleased:
			guiManager.onMouseClick(event.sfmlEvent.mouseButton.x - screenWidth / 2, event.sfmlEvent.mouseButton.y - screenHeight / 2, true);
			break;
		case sf::Event::MouseMoved:
			guiManager.onMouseMove(event.sfmlEvent.mouseMove.x - screenWidth / 2, event.sfmlEvent.mouseMove.y - screenHeight / 2);
			break;
	}

	if(event.sfmlEvent.type == sf::Event::TextEntered && event.sfmlEvent.text.unicode >= 32 && event.sfmlEvent.text.unicode < 128)
	{
		guiManager.onTextEntered(event.sfmlEvent.text.unicode);
	}
	else
	{
		if(event.sfmlEvent.type == sf::Event::KeyPressed)
		{
			guiManager.onSpecialKey(event.sfmlEvent.key.code);
		}
	}
}

MainScene::MainScene(float windowWidth, float windowHeight, std::string address) :
	guiManager(windowWidth, windowHeight),
	console(-windowWidth / 2 + CONSOLE_WIDTH / 2, windowHeight / 2 - CONSOLE_HEIGHT / 2, CONSOLE_WIDTH, CONSOLE_HEIGHT, guiManager),
	controller(console),
	renderSystem(console),
	networkLogic(IpAddress(address, SERVER_PORT), "testActor", controller, renderSystem)
{}

void MainScene::onFrame()
{
	RenderWindow::getInstance()->window.clear();
	//rendering game_logic
	networkLogic.sendEvents();
	renderSystem.onUpdate(networkLogic.receiveUpdates());
	renderSystem.draw();
	//rendering gui...
	guiManager.draw();
	RenderWindow::getInstance()->window.display();
}

void MainScene::onEvent(WindowEvent& event)
{
	controller.onEvent(event.sfmlEvent);
	guiManager.onEvent(event.sfmlEvent);
}

