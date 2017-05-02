#include "render_system.h"

LightManager::LightManager(float screenWidth, float screenHeight, float tileWidth) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	tileWidth(tileWidth),
	counter(0)
{
	shader.loadFromFile(LIGHT_VERTEX_SHADER, LIGHT_FRAGMENT_SHADER);
	multiplyShader.loadFromFile(MULTIPLY_FRAGMENT_SHADER, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / 4, screenHeight / 4);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int LightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	LightSource lightSource;
	sf::Vertex vertex;
	vertex.color = color;
	vertex.color.a = intensity;
	vertex.texCoords = pos;
	vertex.position = sf::Vector2f(-screenWidth / 2, -screenHeight / 2);
	lightSource.vertices[0] = vertex;
	vertex.position = sf::Vector2f(screenWidth / 2, -screenHeight / 2);
	lightSource.vertices[1] = vertex;
	vertex.position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
	lightSource.vertices[2] = vertex;
	vertex.position = sf::Vector2f(-screenWidth / 2, screenHeight / 2);
	lightSource.vertices[3] = vertex;

	verticesMap[counter] = lightSource;

	return counter++;
}

void LightManager::draw(sf::RenderTarget& renderTarget)
{
	sf::RenderStates renderStates;
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendAdd;
	sf::Vector2f center = renderTarget.getView().getCenter();
	center.x *= tileWidth;
	center.y *= tileWidth;
	shader.setParameter("offset", center);

	std::vector<sf::Vertex> vertices;
	for (auto it = verticesMap.begin(); it != verticesMap.end(); it++)
	{
		for (int i = 0; i < 4; i++)
		{
			vertices.push_back(it->second.vertices[i]);
		}
	}

	sf::View lightView;
	lightView.setCenter(sf::Vector2f(0, 0));
	lightView.setSize(screenWidth, screenHeight);
	renderTexture.setView(lightView);
	renderTexture.clear();
	renderTexture.draw(&vertices[0], vertices.size(), sf::Quads, renderStates);
	renderTexture.display();
	shape.setTexture(&renderTexture.getTexture());
	shape.setPosition(renderTarget.getView().getCenter());

	sf::RenderStates multiplyRenderStates;

	multiplyRenderStates.blendMode = sf::BlendAdd;
	multiplyShader.setParameter("multiplier", 5.0f * 0.2f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setParameter("multiplier", 5.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
}

void LightManager::setPosition(int lightSourceID, sf::Vector2f pos)
{
	verticesMap[lightSourceID].vertices[0].texCoords = pos;
	verticesMap[lightSourceID].vertices[1].texCoords = pos;
	verticesMap[lightSourceID].vertices[2].texCoords = pos;
	verticesMap[lightSourceID].vertices[3].texCoords = pos;
}

void LightManager::removeLightSource(int lightSourceID)
{
	verticesMap.erase(lightSourceID);
}