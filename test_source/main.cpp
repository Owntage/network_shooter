#include <SFML/Graphics.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <chrono>
using namespace std;



int main()
{
	RenderWindow::getInstance()->window.setFramerateLimit(60);
	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(), sf::Vector2f(800, 600)));
	
	
	sf::Image image;
	image.loadFromFile("nine_patch_panel2.png");
	NinePatchSprite sprite(image, true);
	MovableWindowPanel windowPanel(0.0, 0.0, 500, 400, sprite);
	GuiManager guiManager(800, 600);
	guiManager.addElement(windowPanel);

	ScrollingTextView textView(0.0f, 0.0f, 0.9, 0.5, "nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png");
	textView.setText("Enter your text: ");
	textView.makeChildOf(windowPanel);
	textView.setColor(sf::Color(100, 50, 0));
	textView.setCharacterSize(24);
	textView.setScaleRelativeToParent(true);
	guiManager.addElement(textView);

	InputField inputField(0.0,0.0, 0.8, 0.1, "button_normal.png", "button_hovered.png", "button_pressed.png");
	inputField.setRelative(0.0, 0.4);
	inputField.setScaleRelativeToParent(true);
	inputField.makeChildOf(windowPanel);
	inputField.setInputCallback([&textView](std::string input)
	{
		textView.setText(textView.getText() + '\n' + input);
	});
	guiManager.addElement(inputField);

	
	auto fpsTime = chrono::system_clock::now() + chrono::seconds(1);
	int fpsCount = 0;

	while(RenderWindow::getInstance()->window.isOpen())
	{
		fpsCount++;
		if(chrono::system_clock::now() > fpsTime)
		{
			std::cout << "fps: " << fpsCount << std::endl;
			fpsTime = fpsTime + chrono::seconds(1);
			fpsCount = 0;
		}


		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					RenderWindow::getInstance()->window.close();
					break;
				default:
					guiManager.onEvent(event);
					break;
			}
		}
		RenderWindow::getInstance()->window.clear();
		guiManager.draw();
		RenderWindow::getInstance()->window.display();
		
	}
	return 0;
}