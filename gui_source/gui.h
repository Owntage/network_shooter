#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <functional>
#include "singleton.h"

#define FONT_NAME "res/UbuntuMono-R.ttf"

struct GuiElement
{
	
	GuiElement(float x, float y, float scaleX, float scaleY): x(x), y(y), scaleX(scaleX), scaleY(scaleY),
		isRelativeScale(false), relativeX(0.0), relativeY(0.0), parent(nullptr), usesMouse(false) {}
	GuiElement(): x(0.0f), y(0.0f), scaleX(0.0f), scaleY(0.0f), isRelativeScale(false), relativeX(0.0f), relativeY(0.0f), parent(nullptr),
		usesMouse(false) {}
	virtual void draw(sf::RenderTarget& renderTarget) = 0;
	virtual void onMouseMove(float x, float y) {}
	virtual void onMouseClick(float x, float y, bool isReleased) {}
	void makeChildOf(GuiElement& other);
	void setX(float x);
	void setY(float y);
	void setRelativeX(float relativeX);
	void setRelativeY(float relativeY);
	void set(float x, float y);
	void setRelative(float relativeX, float relativeY);
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);
	void setScale(float scaleX, float scaleY);
	void setScaleRelativeToParent(bool isRelativeScale);
	float getX();
	float getY();
	float getScaleX();
	float getScaleY();
	GuiElement* getParent();
	bool usesMouse;
protected:
	virtual ~GuiElement() {}
	float getRelativeToParentX() { return x; }
	float getRelativeToParentY() { return y; }
	
private:
	float x;
	float y;
	float relativeX;
	float relativeY;
	float scaleX;
	float scaleY;
	bool isRelativeScale;
	GuiElement* parent;
};

struct GuiManager
{
	GuiManager(float screenWidth, float screenHeight) : guiView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(screenWidth, screenHeight))
	{
		renderTexture.create(screenWidth, screenHeight);
		renderTexture.setView(guiView);
	}
	void addElement(GuiElement& element);
	void draw();
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	void drawGuiElement(GuiElement* element);
	std::map<GuiElement*, bool> isDrawn;
	std::map<GuiElement*, int> renderOrder;
	int renderCounter;
	sf::View guiView;
	sf::RenderTexture renderTexture;
	std::vector<GuiElement*> elements;
};

struct NinePatchSprite
{
	NinePatchSprite(sf::Image& image, bool isNinePatch);
	NinePatchSprite() : isNinePatch(false) {}
	void setTexture(sf::Image& image, bool isNinePatch);
	void setX(float x);
	void setY(float y);
	void set(float x, float y);
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);
	void setScale(float scaleX, float scaleY);
	void draw(sf::RenderTarget& renderTarget);
	
private:
	sf::Texture texture;
	sf::RectangleShape shapes[3][3];
	float x;
	float y;
	float scaleX;
	float scaleY;
	int firstFixedSizeX;
	int secondFixedSizeX;
	int firstFixedSizeY;
	int secondFixedSizeY;
	bool isNinePatch;
};

struct WindowPanel : GuiElement
{
	WindowPanel(float x, float y, float scaleX, float scaleY, NinePatchSprite& background) : GuiElement(x, y, scaleX, scaleY),
		activeSprite(&background) {}
	void draw(sf::RenderTarget& renderTarget);
	virtual NinePatchSprite& getActiveSprite();
protected:
	WindowPanel(float x, float y, float scaleX, float scaleY) : GuiElement(x, y, scaleX, scaleY), activeSprite(nullptr) {}
private:
	NinePatchSprite* activeSprite;
};

struct MovableWindowPanel : WindowPanel
{
	MovableWindowPanel(float x, float y, float scaleX, float scaleY, NinePatchSprite& background) : 
		WindowPanel(x, y, scaleX, scaleY, background),	isCaptured(false) { usesMouse = true; }
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	bool isCaptured;
	float lastMouseX;
	float lastMouseY;
};

struct Button : WindowPanel
{
	Button(float x, float y, float scaleX, float scaleY) : WindowPanel(x, y, scaleX, scaleY),
		normal(nullptr), hovered(nullptr), pressed(nullptr), state(ButtonState::normal),
		onPressCallback([](){}), onReleaseCallback([](){})	{ usesMouse = true; }
	NinePatchSprite& getActiveSprite();
	void setNormalSprite(NinePatchSprite& normalSprite);
	void setHoveredSprite(NinePatchSprite& hoveredSprite);
	void setPressedSprite(NinePatchSprite& pressedSprite);
	void setOnPressCallback(std::function<void(void)>&& onPressCallback);
	void setOnReleaseCallback(std::function<void(void)>&& onReleaseCallback);
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	NinePatchSprite* normal;
	NinePatchSprite* hovered;
	NinePatchSprite* pressed;
	std::function<void(void)> onPressCallback;
	std::function<void(void)> onReleaseCallback;
	enum class ButtonState
	{
		normal,
		hovered,
		pressed
	};
	ButtonState state;
	
};

struct Font : Singleton<Font>
{
	sf::Font font;
	float characterHeight;
	Font()
	{
		font.loadFromFile(FONT_NAME);
	}
};

struct TextView : GuiElement
{
	TextView(float x, float y, float scaleX, float scaleY) : GuiElement(x, y, scaleX, scaleY), color(sf::Color::Green), characterSize(16) {}
	void draw(sf::RenderTarget& renderTarget);
	void setText(std::string text);
	void setCharacterSize(int size);
	void setColor(sf::Color color);
	std::string getText();
private:
	std::string text;
	int characterSize;
	sf::Color color;
};

struct InputField : GuiElement //todo...
{
	InputField(float x, float y, float scaleX, float scaleY) : GuiElement(x, y, scaleX, scaleY) {}
private:
	void onMouseClick(float x, float y, bool isReleased);
	
};

#endif