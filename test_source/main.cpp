#include <SFML/Graphics.hpp>
#include "render_window.h"
#include "gui.h"
#include <memory>
using namespace std;

GuiManager guiManager(WINDOW_WIDTH, WINDOW_HEIGHT);

#define NORMAL "res/gui/console/background.png"
#define HOVERED "res/gui/console/input_hovered_background.png"
#define PRESSED "res/gui/console/input_background.png"

#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 10

#define CELL_SIZE 35
#define CELL_SPACING 5

double matrixValues[MATRIX_WIDTH][MATRIX_HEIGHT];

NinePatchSprite normalSprite(NORMAL, true);
NinePatchSprite hoveredSprite(HOVERED, true);
NinePatchSprite pressedSprite(PRESSED, true);

vector<shared_ptr<TextView> > labels;
vector<vector<shared_ptr<InputField> > > matrixCells;

shared_ptr<Button> createButton(
	float x,
	float y,
	float sizeX,
	float sizeY,
	std::string labelText
)
{
	auto res = make_shared<Button>(x, y, sizeX, sizeY);
	res->setNormalSprite(normalSprite);
	res->setHoveredSprite(hoveredSprite);
	res->setPressedSprite(pressedSprite);

	guiManager.addElement(*res);

	auto label = make_shared<TextView>(0, 0, 200, 50);
	label->setText(labelText);
	label->setColor(sf::Color::White);
	label->makeChildOf(*res);
	guiManager.addElement(*label);
	labels.push_back(label);
	return res;
}

shared_ptr<InputField> createInputField(float x, float y)
{
	auto res = make_shared<InputField>(x, y, CELL_SIZE, CELL_SIZE, NORMAL, HOVERED, PRESSED);
	guiManager.addElement(*res);
	res->setCharacterSize(10);
	return res;
}

pair<float, float> createMatrix()
{
	float width = CELL_SIZE * MATRIX_WIDTH + CELL_SPACING * (MATRIX_WIDTH - 1);
	float height = CELL_SIZE * MATRIX_HEIGHT + CELL_SPACING * (MATRIX_WIDTH - 1);
	matrixCells.resize(MATRIX_WIDTH);
	for (int i = 0; i < MATRIX_WIDTH; i++)
	{
		matrixCells[i].resize(MATRIX_HEIGHT);
	}
		
	for (int i = 0; i < MATRIX_WIDTH; i++)
	{
		for(int j = 0; j < MATRIX_HEIGHT; j++)
		{
			matrixCells[i][j] = createInputField(
				CELL_SIZE * i + CELL_SPACING * (i - 1) - width / 2 + CELL_SIZE, 
				CELL_SIZE * j + CELL_SPACING * (j - 1) - height / 2 + CELL_SIZE);
		}
	}
	return make_pair(width, height);
}

int main()
{
	//auto button1 = createButton(50, 50, 200, 50, "test label");

	//auto input1 = createInputField(-100, -100);
	createMatrix();

	while(RenderWindow::getInstance()->window.isOpen())
	{
		RenderWindow::getInstance()->window.clear();
		guiManager.draw();
		RenderWindow::getInstance()->window.display();
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}			
			guiManager.onEvent(event);
		}
	}
	return 0;
}