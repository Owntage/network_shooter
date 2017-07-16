#include "render_system.h"
#include "render_window.h"

LightManager::LightManager(float screenWidth, float screenHeight, float tileWidth) :
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	tileWidth(tileWidth),
	counter(0),
	shaderArraySize(0)
{

	sf::FileInputStream vertexStream;
	vertexStream.open(LIGHT_VERTEX_SHADER);
	sf::FileInputStream fragmentStream;
	fragmentStream.open(LIGHT_FRAGMENT_SHADER);
	shader.loadFromStream(vertexStream, fragmentStream);

	sf::FileInputStream multiplyFragmentStream;
	multiplyFragmentStream.open(MULTIPLY_FRAGMENT_SHADER);
	multiplyShader.loadFromStream(multiplyFragmentStream, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / 4, screenHeight / 4);
	renderTexture.setSmooth(true);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int LightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	idToShaderIndex[counter] = shaderArraySize;
	shader.setUniform("light_pos[" + std::to_string(shaderArraySize) + "]", pos);

	sf::Vector3f colorVec(color.r, color.g, color.b);
	shader.setUniform("light_color[" + std::to_string(shaderArraySize) + "]", colorVec);

	shader.setUniform("light_intensity[" + std::to_string(shaderArraySize) + "]", intensity);

	shader.setUniform("sources_size", ++shaderArraySize);
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
	shader.setUniform("offset", center);

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
	multiplyShader.setUniform("multiplier", 5.0f * 0.2f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setUniform("multiplier", 5.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
}

void LightManager::setPosition(int lightSourceID, sf::Vector2f pos)
{
	shader.setUniform("light_pos[" + std::to_string(idToShaderIndex[lightSourceID]) + "]", pos);
}

void LightManager::removeLightSource(int lightSourceID)
{
	//verticesMap.erase(lightSourceID);
}

void LightManager::onWindowResize(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}