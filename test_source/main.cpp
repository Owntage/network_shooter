#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <chrono>
#include <sstream>
#include <network.h>
using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100


#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"
#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"

struct LightManager
{
	LightManager(float screenWidth, float screenHeight) :
		screenWidth(screenWidth),
		screenHeight(screenHeight)
	{
		shader.loadFromFile(LIGHT_VERTEX_SHADER, LIGHT_FRAGMENT_SHADER);
		vertices.setPrimitiveType(sf::Quads);
		renderTexture.create(screenWidth / 4, screenHeight / 4);
		
		shape.setSize(sf::Vector2f(screenWidth, screenHeight));
		shape.setOrigin(screenWidth / 2, screenHeight / 2);
	}
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
	{
		sf::Vertex vertex;
		vertex.color = color;
		vertex.color.a = intensity;
		vertex.texCoords = pos;
		vertex.position = sf::Vector2f(-screenWidth / 2, -screenHeight / 2);
		vertices.append(vertex);
		vertex.position = sf::Vector2f(screenWidth / 2, -screenHeight / 2);
		vertices.append(vertex);
		vertex.position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
		vertices.append(vertex);
		vertex.position = sf::Vector2f(-screenWidth / 2, screenHeight / 2);
		vertices.append(vertex);
		return vertices.getVertexCount() / 4 - 1;
	}
	void draw(sf::RenderTarget& renderTarget)
	{
		sf::RenderStates renderStates;
		renderStates.shader = &shader;
		renderStates.blendMode = sf::BlendAdd;
		shader.setParameter("offset", renderTarget.getView().getCenter());
		renderTexture.setView(renderTarget.getView());
		renderTexture.clear();
		renderTexture.draw(vertices, renderStates);
		renderTexture.display();
		shape.setTexture(&renderTexture.getTexture());
		shape.setPosition(renderTarget.getView().getCenter());
		renderTarget.draw(shape);
	}
	void setPosition(int lightSourceIndex, sf::Vector2f pos)
	{
		vertices[lightSourceIndex * 4].texCoords = pos;
		vertices[lightSourceIndex * 4 + 1].texCoords = pos;
		vertices[lightSourceIndex * 4 + 2].texCoords = pos;
		vertices[lightSourceIndex * 4 + 3].texCoords = pos;
	}
	void removeLightSource(int lightSourceIndex)
	{
		std::copy(&vertices[vertices.getVertexCount() - 4], &vertices[vertices.getVertexCount() - 4] + 4, &vertices[lightSourceIndex * 4]);
		vertices.resize(vertices.getVertexCount() - 4);
	}
private:
	sf::VertexArray vertices;
	sf::Shader shader;
	sf::RectangleShape shape;
	sf::RenderTexture renderTexture;
	float screenWidth;
	float screenHeight;
};


int main()
{
	LightManager lightManager(800, 600);
	for(int i = 0; i < 50; i++)
	{
		int x = (rand() % 100) - 50;
		int y = (rand() % 100) - 50;
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		int intensity = rand() % 10 + 1;
		lightManager.addLightSource(sf::Vector2f(x, y), sf::Color(r, g, b), intensity);
	}

	int specialLightSource = lightManager.addLightSource(sf::Vector2f(0, 0), sf::Color::Cyan, 20);
	float phase = 0;

	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(100.0, 0.0), sf::Vector2f(800, 600)));
	RenderWindow::getInstance()->window.setFramerateLimit(60);

	int frameCount = 0;
	bool deleteFlag = false;

	while(RenderWindow::getInstance()->window.isOpen())
	{
		phase += 3.14 / 60.0;
		frameCount++;
		lightManager.setPosition(specialLightSource, sf::Vector2f(cos(phase) * 400.0f, sin(phase) * 300.0f));
		
		if(frameCount > 500 && !deleteFlag)
		{
			lightManager.removeLightSource(specialLightSource);
			deleteFlag = true;
		}


		RenderWindow::getInstance()->window.clear();
		lightManager.draw(RenderWindow::getInstance()->window);
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