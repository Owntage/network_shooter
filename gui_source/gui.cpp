#include "gui.h"
#include "render_window.h"
#include <iostream>
#include <algorithm>
#include <utility>
using namespace std;

void GuiElement::makeChildOf(GuiElement& other)
{
	parent = &other;
}

void GuiElement::setX(float x)
{
	this->x = x;
}

void GuiElement::setY(float y)
{
	this->y = y;
}

void GuiElement::setRelativeX(float relativeX)
{
	this->relativeX = relativeX;
}

void GuiElement::setRelativeY(float relativeY)
{
	this->relativeY = relativeY;
}

void GuiElement::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void GuiElement::setRelative(float relativeX, float relativeY)
{
	this->relativeX = relativeX;
	this->relativeY = relativeY;
}

void GuiElement::setScaleX(float scaleX)
{
	this->scaleX = scaleX;
}

void GuiElement::setScaleY(float scaleY)
{
	this->scaleY = scaleY;
}

void GuiElement::setScale(float scaleX, float scaleY)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void GuiElement::setScaleRelativeToParent(bool isRelativeScale)
{
	this->isRelativeScale = isRelativeScale;
}

float GuiElement::getX()
{
	if(parent != nullptr)
	{
		return parent->getX() + x + parent->getScaleX() * relativeX;
		
	}
	return x;
}

float GuiElement::getY()
{
	if(parent != nullptr)
	{
		return parent->getY() + y + parent->getScaleY() * relativeY;
	}
	return y;
}

float GuiElement::getScaleX()
{
	float parentScaleX = 1.0f;
	if(parent != nullptr && isRelativeScale)
	{
		parentScaleX = parent->getScaleX();
	}
	return parentScaleX * scaleX;
}

float GuiElement::getScaleY()
{
	float parentScaleY = 1.0f;
	if(parent != nullptr && isRelativeScale)
	{
		parentScaleY = parent->getScaleY();
	}
	return parentScaleY * scaleY;
}

GuiElement* GuiElement::getParent()
{
	return parent;
}

void GuiManager::addElement(GuiElement& element)
{
	elements.push_back(&element);
	isDrawn[&element] = false;
	renderOrder[&element] = -1;
}

void GuiManager::drawGuiElement(GuiElement* guiElement)
{
	if(guiElement->getParent() != nullptr)
	{
		drawGuiElement(guiElement->getParent());
	}
	if(!isDrawn[guiElement])
	{
		
		renderTexture.clear(sf::Color::Transparent);
		guiElement->draw(renderTexture);
		renderTexture.display();
		
		sf::Sprite renderSprite(renderTexture.getTexture());
		
		sf::IntRect textureRect;
		textureRect.width = guiElement->getScaleX();
		textureRect.height = guiElement->getScaleY();
		textureRect.left = guiElement->getX() - guiElement->getScaleX() / 2 + guiView.getSize().x / 2;
		textureRect.top = guiElement->getY() - guiElement->getScaleY() / 2 + guiView.getSize().y / 2;
		renderSprite.setTextureRect(textureRect);

		renderSprite.setOrigin(guiElement->getScaleX() / 2, guiElement->getScaleY() / 2);
		renderSprite.setPosition(guiElement->getX(), guiElement->getY());
		
		RenderWindow::getInstance()->window.draw(renderSprite);
		
		isDrawn[guiElement] = true;
		renderOrder[guiElement] = renderCounter;
		renderCounter++;
	}
}

void GuiManager::draw()
{
	
	renderCounter = 1;
	RenderWindow::getInstance()->window.setView(guiView);
	for(auto it = isDrawn.begin(); it != isDrawn.end(); it++)
	{
		it->second = false;
	}
	
	//sf::View defaultView = guiView;
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		
		
		drawGuiElement(*it);
	}
	
	
}

void GuiManager::onMouseMove(float x, float y)
{
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		(*it)->onMouseMove(x, y);
	}
}

bool isInRectangle(float x, float y, float rectangleCenterX, float rectangleCenterY, float scaleX, float scaleY)
{
	if(x > rectangleCenterX + scaleX / 2) return false;
	if(x < rectangleCenterX - scaleX / 2) return false;
	if(y > rectangleCenterY + scaleY / 2) return false;
	if(y < rectangleCenterY - scaleY / 2) return false;
	return true;
}

void GuiManager::onMouseClick(float x, float y, bool isReleased)
{
	if(isReleased)
	{
		for(auto it = elements.begin(); it != elements.end(); it++)
		{
			(*it)->onMouseClick(x, y, isReleased);
		}
	}
	else
	{
		vector<GuiElement*> clickedElements;
		for(auto it = elements.begin(); it != elements.end(); it++)
		{
			if(isInRectangle(x, y, (*it)->getX(), (*it)->getY(), (*it)->getScaleX(), (*it)->getScaleY()) && (*it)->usesMouse)
			{
				clickedElements.push_back(*it);
			}
		}
		std::sort(clickedElements.begin(), clickedElements.end(), [this](GuiElement* first, GuiElement* second) -> bool
		{
			return this->renderOrder[first] > this->renderOrder[second];
		});
		if(clickedElements.size() > 0)
		{
			clickedElements[0]->onMouseClick(x, y, isReleased);
		}
	}
}


NinePatchSprite::NinePatchSprite(sf::Image& image, bool isNinePatch)
{
	setTexture(image, isNinePatch);
}

void NinePatchSprite::setTexture(sf::Image& image, bool isNinePatch)
{
	texture.loadFromImage(image);
	this->isNinePatch = isNinePatch;
	//TODO CUTTING AN IMAGE
	if(isNinePatch)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				shapes[i][j].setTexture(&texture);
				
			}
		}
		int imageWidth = image.getSize().x;
		int imageHeight = image.getSize().y;
		
		//reading for x projection
		bool readingFirst = true;
		for(int i = 1; i < imageWidth - 1; i++)
		{
			sf::Color color = image.getPixel(i, 0);
			if(readingFirst == true && color != sf::Color::Black)
			{
				readingFirst = false;
				firstFixedSizeX = i - 1;
			}
			if(readingFirst == false && color == sf::Color::Black)
			{
				secondFixedSizeX = imageWidth - i - 1;
				break;
			}
		}
		
		
		
		//reading for y projection
		readingFirst = true;
		for(int i = 1; i < imageHeight - 1; i++)
		{
			sf::Color color = image.getPixel(0, i);
			if(readingFirst == true && color != sf::Color::Black)
			{
				readingFirst = false;
				firstFixedSizeY = i - 1;
			}
			if(readingFirst == false && color == sf::Color::Black)
			{
				secondFixedSizeY = imageHeight - i - 1;
				break;
			}
		}
		
		
		//setting texture rects
		int middleSizeX = imageWidth - firstFixedSizeX - secondFixedSizeX - 2;
		int middleSizeY = imageHeight - firstFixedSizeY - secondFixedSizeY - 2;
		
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				int sizeX;
				int sizeY;
				int offsetX;
				int offsetY;
				switch(i)
				{
					case 0: sizeX = firstFixedSizeX; offsetX = 1; break;
					case 1: sizeX = middleSizeX; offsetX = 1 + firstFixedSizeX; break;
					case 2: sizeX = secondFixedSizeX; offsetX = 1 + firstFixedSizeX + middleSizeX; break;
				}
				switch(j)
				{
					case 0: sizeY = firstFixedSizeY; offsetY = 1; break; 
					case 1: sizeY = middleSizeY; offsetY = 1 + firstFixedSizeY; break;
					case 2: sizeY = secondFixedSizeY; offsetY = 1 + firstFixedSizeY + middleSizeY; break;
				}
				shapes[i][j].setTextureRect(sf::IntRect(offsetX, offsetY, sizeX, sizeY));
			}
		}
		
	}
	else
	{
		shapes[1][1].setTexture(&texture);
	}
}

void NinePatchSprite::setX(float x)
{
	this->x = x;
}

void NinePatchSprite::setY(float y)
{
	this->y = y;
}

void NinePatchSprite::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void NinePatchSprite::setScaleX(float scaleX)
{
	this->scaleX = scaleX;
}

void NinePatchSprite::setScaleY(float scaleY)
{
	this->scaleY = scaleY;
}

void NinePatchSprite::setScale(float scaleX, float scaleY)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void NinePatchSprite::draw(sf::RenderTarget& renderTarget)
{
	if(isNinePatch)
	{
		float firstSizeX = (float) firstFixedSizeX;
		float firstSizeY = (float) firstFixedSizeY;
		float secondSizeX = (float) secondFixedSizeX;
		float secondSizeY = (float) secondFixedSizeY;
		float middleSizeX = scaleX - firstSizeX - secondSizeX;
		float middleSizeY = scaleY - firstSizeY - secondSizeY;
	
		
		//setting sizes, positions and origins. rendering
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				float sizeX;
				float sizeY;
				float positionX;
				float positionY;
				switch(i)
				{
					case 0: sizeX = firstSizeX; positionX = -middleSizeX / 2.0f - firstSizeX / 2.0f; break;
					case 1: sizeX = middleSizeX; positionX = 0; break;
					case 2: sizeX = secondSizeX; positionX = middleSizeX / 2.0f + secondSizeX / 2.0f; break;
				}
				switch(j)
				{
					case 0: sizeY = firstSizeY; positionY = -middleSizeY / 2.0f - firstSizeY / 2.0f; break;
					case 1: sizeY = middleSizeY; positionY = 0; break;
					case 2: sizeY = secondSizeY; positionY = middleSizeY / 2.0f + secondSizeY / 2.0f; break;
				}
				shapes[i][j].setOrigin(sizeX / 2.0f, sizeY / 2.0f);
				shapes[i][j].setSize(sf::Vector2f(sizeX, sizeY));
				shapes[i][j].setPosition(x + positionX, y + positionY);
				
				//RenderWindow::getInstance()->window.draw(shapes[i][j]);
				renderTarget.draw(shapes[i][j]);
			}
		}
		
		
	}
	else
	{
		shapes[1][1].setSize(sf::Vector2f(scaleX, scaleY));
		shapes[1][1].setPosition(x, y);
		//RenderWindow::getInstance()->window.draw(shapes[1][1]);
		renderTarget.draw(shapes[1][1]);
	}
}

NinePatchSprite& WindowPanel::getActiveSprite()
{
	return *activeSprite;
}

void WindowPanel::draw(sf::RenderTarget& renderTarget)
{
	NinePatchSprite& sprite = getActiveSprite();
	sprite.set(getX(), getY());
	sprite.setScale(getScaleX(), getScaleY());
	sprite.draw(renderTarget);
}

void MovableWindowPanel::onMouseClick(float x, float y, bool isReleased)
{
	if(isReleased)
	{
		isCaptured = false;
	}
	else
	{
		isCaptured = true;
		lastMouseX = x;
		lastMouseY = y;
	}
}

void MovableWindowPanel::onMouseMove(float x, float y)
{
	if(isCaptured)
	{
		setX(getRelativeToParentX() + x - lastMouseX);
		setY(getRelativeToParentY() + y - lastMouseY);
		lastMouseX = x;
		lastMouseY = y;
	}
}

NinePatchSprite& Button::getActiveSprite()
{
	switch(state)
	{
		case ButtonState::normal: return *normal; break;
		case ButtonState::hovered: return *hovered; break;
		case ButtonState::pressed: return *pressed; break;
	}
}

void Button::setNormalSprite(NinePatchSprite& normal)
{
	this->normal = &normal;
}

void Button::setHoveredSprite(NinePatchSprite& hovered)
{
	this->hovered = &hovered;
}

void Button::setPressedSprite(NinePatchSprite& pressed)
{
	this->pressed = &pressed;
}

void Button::setOnPressCallback(std::function<void(void)>&& onPressCallback)
{
	this->onPressCallback = onPressCallback;
}

void Button::setOnReleaseCallback(std::function<void(void)>&& onReleaseCallback)
{
	this->onReleaseCallback = onReleaseCallback;
}

void Button::onMouseMove(float x, float y)
{
	switch(state)
	{
		case ButtonState::normal:
			if(isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::hovered;
			}
			break;
		case ButtonState::hovered:
			if(!isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::normal;
			}
			break;
		case ButtonState::pressed:
			if(!isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::normal;
			}
			break;
	}
}

void Button::onMouseClick(float x, float y, bool isReleased)
{
	switch(state)
	{
		case ButtonState::normal:
			if(!isReleased)
			{
				state = ButtonState::pressed;
				onPressCallback();
			}
			break;
		case ButtonState::hovered:
			if(!isReleased)
			{
				state = ButtonState::pressed;
				onPressCallback();
			}
			break;
		case ButtonState::pressed:
			if(isReleased)
			{
				state = ButtonState::hovered;
				onReleaseCallback();
			}
			break;
	}
}

void TextView::draw(sf::RenderTarget& renderTarget)
{
	float singleCharacterSize;
	float characterHeight;

	sf::Text oneCharacter;
	oneCharacter.setCharacterSize(characterSize);
	oneCharacter.setFont(Font::getInstance()->font);
	oneCharacter.setString("R");
	singleCharacterSize = oneCharacter.getLocalBounds().width;
	characterHeight = oneCharacter.getLocalBounds().height;
	int charactersPerLine = getScaleX() / singleCharacterSize;

	
	int counter = 0;
	
	for(int i = 0; i < this->text.size(); i++)
	{
		if(this->text[i] == '\n')
		{
			counter = 0;
		}
		else
		{
			counter++;
		}
		if(counter > charactersPerLine)
		{
			this->text.insert(this->text.begin() + i, '\n');
			//i+=2;
			counter = 0;
		}
	}

	

	//this->text.erase(this->text.begin() + min(charactersPerLine * ((int) (getScaleY() / 2 / characterHeight)), (int) this->text.size()), this->text.end());
	

	sf::Text text;
	text.setFont(Font::getInstance()->font);
	text.setString(this->text);
	text.setCharacterSize(characterSize);
	text.setColor(color);
	text.setPosition(getX(), getY());
	text.setOrigin(getScaleX() / 2, getScaleY() / 2);
	
	
	//RenderWindow::getInstance()->window.draw(text);
	renderTarget.draw(text);
	
	
}

void TextView::setText(std::string text)
{
	this->text = text;
}

void TextView::setCharacterSize(int characterSize)
{
	this->characterSize = characterSize;
}

void TextView::setColor(sf::Color color)
{
	this->color = color;
}

std::string TextView::getText()
{
	return text;
}