#include <SFML/Graphics.hpp>
#include "render_window.h"
#include "gui.h"
using namespace std;


int main()
{
	while(RenderWindow::getInstance()->window.isOpen())
	{
		RenderWindow::getInstance()->window.clear();
		RenderWindow::getInstance()->window.display();
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
		}
	}
	return 0;
}