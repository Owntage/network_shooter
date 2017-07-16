#include "render_system.h"
#include "render_window.h"

LightManager::LightManager(float screenWidth, float screenHeight, float tileWidth) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	tileWidth(tileWidth),
	counter(0),
	shaderArraySize(0)
{
	shader.loadFromFile(LIGHT_VERTEX_SHADER, LIGHT_FRAGMENT_SHADER);
	multiplyShader.loadFromFile(MULTIPLY_FRAGMENT_SHADER, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / 4, screenHeight / 4);
	renderTexture.setSmooth(true);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int LightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	idToShaderIndex[counter] = shaderArraySize;
	shader.setParameter("light_pos[" + std::to_string(shaderArraySize) + "]", pos);

	sf::Vector3f colorVec(color.r, color.g, color.b);
	shader.setParameter("light_color[" + std::to_string(shaderArraySize) + "]", colorVec);

	shader.setParameter("light_intensity[" + std::to_string(shaderArraySize) + "]", intensity);

	shader.setParameter("sources_size", ++shaderArraySize);
	return counter++;
}

void LightManager::draw(sf::RenderTarget& renderTarget)
{
	float actualWindowWidth = RenderWindow::getInstance()->window.getSize().x;
	float actualWindowHeight = RenderWindow::getInstance()->window.getSize().y;
	if(actualWindowWidth != screenWidth || actualWindowHeight != screenHeight)
	{
		onWindowResize(actualWindowWidth, actualWindowHeight);
	}

	sf::RenderStates renderStates;
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendAdd;
	sf::Vector2f center = renderTarget.getView().getCenter();
	center.x *= tileWidth;
	center.y *= tileWidth;
	shader.setParameter("offset", center);

	std::vector<sf::Vertex> vertices;
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, screenHeight / 2)));

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
	shader.setParameter("light_pos[" + std::to_string(idToShaderIndex[lightSourceID]) + "]", pos);
}

void LightManager::removeLightSource(int lightSourceID)
{
	//verticesMap.erase(lightSourceID);
}

void LightManager::onWindowResize(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	//for(auto it = verticesMap.begin(); it != verticesMap.end(); it++)
	//{
	//	it->second.vertices[0].position = sf::Vector2f(-screenWidth / 2, -screenHeight / 2);
	//	it->second.vertices[1].position = sf::Vector2f(screenWidth / 2, -screenHeight / 2);
	//	it->second.vertices[2].position = sf::Vector2f(screenWidth / 2, screenHeight / 2);
	//	it->second.vertices[3].position = sf::Vector2f(-screenWidth / 2, screenHeight / 2);
	//}
	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}