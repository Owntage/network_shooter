#include <SFML/Graphics.hpp>
#include "render_window.h"
#include "gui.h"
using namespace std;


int main()
{
	GuiManager guiManager(WINDOW_WIDTH, WINDOW_HEIGHT);



	while(RenderWindow::getInstance()->window.isOpen())
	{
		RenderWindow::getInstance()->window.clear();
		RenderWindow::getInstance()->window.display();
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				guiManager.onMouseClick(event.mouseButton.x - WINDOW_WIDTH / 2, event.mouseButton.y - WINDOW_HEIGHT / 2, false);
				break;
			case sf::Event::MouseButtonReleased:
				guiManager.onMouseClick(event.mouseButton.x - WINDOW_WIDTH / 2, event.mouseButton.y - WINDOW_HEIGHT / 2, true);
				break;
			case sf::Event::MouseMoved:
				guiManager.onMouseMove(event.mouseMove.x - WINDOW_WIDTH / 2, event.mouseMove.y - WINDOW_HEIGHT / 2);
				break;
			case sf::Event::Closed:
				RenderWindow::getInstance()->window.close();
				break;
			default:
				break;
			}
		}
	}
	return 0;
}