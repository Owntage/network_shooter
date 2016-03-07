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

	ScrollingTextView textView(0.0f, 0.0f, 0.9, 0.9, "nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png","nine_patch_panel2.png");
	textView.setText("Enter your text: ");
	textView.makeChildOf(windowPanel);
	textView.setColor(sf::Color(100, 50, 0));
	textView.setCharacterSize(24);
	textView.setScaleRelativeToParent(true);
	//textView.setRelativeX(0.5f);
	guiManager.addElement(textView);

	/*
	Slider slider(0, 0, 0.2, 0.8, false, "nine_patch_panel2.png", "nine_patch_panel2.png", 30);
	slider.setMoveCallback([](float pos)
	{
		std::cout << "slider moved: " << pos << std::endl;
	});
	slider.makeChildOf(windowPanel);
	slider.setScaleRelativeToParent(true);
	guiManager.addElement(slider);
	*/

	
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
				case sf::Event::MouseButtonPressed:
					guiManager.onMouseClick(event.mouseButton.x - 400, event.mouseButton.y - 300, false);
					break;
				case sf::Event::MouseButtonReleased:
					guiManager.onMouseClick(event.mouseButton.x - 400, event.mouseButton.y - 300, true);
					break;
				case sf::Event::MouseMoved:
					guiManager.onMouseMove(event.mouseMove.x - 400, event.mouseMove.y - 300);
					break;
				//case sf::Event::TextEntered:
				//	textView.setText(textView.getText() + (char) event.text.unicode);
				//	break;
			}
			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
			{
				string newText = textView.getText();
				newText.pop_back();
				textView.setText(newText);
				std::cout << "removed character " << std::endl;
			}
			else
			{
				if(event.type == sf::Event::TextEntered && event.text.unicode >= 32 && event.text.unicode <= 126)
				{
					
					textView.setText(textView.getText() + (char) event.text.unicode);
				}
			}
		}
		RenderWindow::getInstance()->window.clear();
		guiManager.draw();
		RenderWindow::getInstance()->window.display();
		
	}
	return 0;
}