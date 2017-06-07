#include <SFML/Graphics.hpp>
#include "render_window.h"
#include "gui.h"
#include "matrix_utils.h"
#include <memory>
using namespace std;

GuiManager guiManager(WINDOW_WIDTH, WINDOW_HEIGHT);

#define NORMAL "res/gui/console/background.png"
#define HOVERED "res/gui/console/input_hovered_background.png"
#define PRESSED "res/gui/console/input_background.png"

#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 11

#define CELL_SIZE 35 * 2
#define CELL_SPACING 5


vector<vector<double> > matrixValues = []()
{
	vector<vector<double> > res;
	res.resize(MATRIX_WIDTH);
	for(int i = 0; i < MATRIX_WIDTH; i++)
	{
		res[i].resize(MATRIX_HEIGHT);
	}
	return res;
}();

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

	auto label = make_shared<TextView>(0, 0, sizeX - 10, sizeY - 10);
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
	res->setCharacterSize(16);
	return res;
}

pair<float, float> createMatrix(float x, float y)
{
	float height = CELL_SIZE * MATRIX_WIDTH + CELL_SPACING * (MATRIX_WIDTH - 1);
	float width = CELL_SIZE * MATRIX_HEIGHT + CELL_SPACING * (MATRIX_HEIGHT - 1);
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
				CELL_SIZE * j + CELL_SPACING * (j - 1) - width / 2 + CELL_SIZE + x, 
				CELL_SIZE * i + CELL_SPACING * (i - 1) - height / 2 + CELL_SIZE + y);
		}
	}
	return make_pair(width, height);
}

bool copyErrorFlag = false;
void copyFromGui()
{
	copyErrorFlag = false;
	for(int i = 0; i < MATRIX_WIDTH; i++)
	{
		for(int j = 0; j < MATRIX_HEIGHT; j++)
		{
			try
			{
				matrixValues[i][j] = stof(matrixCells[i][j]->getText());
			}
			catch (...)
			{
				copyErrorFlag = true;
				return;
			} //failed to parse
		}
	}
}

void copyToGui()
{
	for(int i = 0; i < MATRIX_WIDTH; i++)
	{
		for(int j = 0; j < MATRIX_HEIGHT; j++)
		{
			string temp = to_string(matrixValues[i][j]);
			if (temp[0] == '-') temp = temp.substr(1);
			while ((temp.back() == '0' || temp.back() == '.') && temp.size() > 1) temp.pop_back();
			matrixCells[i][j]->setText(temp);
		}
	}
}

int offset = -100;
int getOffset()
{
	offset += CELL_SIZE + CELL_SPACING;
	return offset;
}

matrix_t buffer;


int main()
{
	//auto button1 = createButton(50, 50, 200, 50, "test label");

	//auto input1 = createInputField(-100, -100);
	float width = CELL_SIZE * MATRIX_HEIGHT + CELL_SPACING * (MATRIX_HEIGHT - 1);
	auto matrixSize = createMatrix(-width / 2, 0);
	float remainingWidth = WINDOW_WIDTH - matrixSize.first - 50;
	float elemX = WINDOW_WIDTH / 2 - remainingWidth / 2;
	TextView result(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE);
	result.setColor(sf::Color::White);
	result.setText("result: ");
	guiManager.addElement(result); 

	

	auto gaussButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "gauss");
	gaussButton->setOnReleaseCallback([&]()
	{
		copyFromGui();
		if(!copyErrorFlag)
		{
			auto temp = matrixValues;
			auto res = gauss(matrixValues);
			double cond = norm(temp) * norm(res.second);
			//double cond = norm(generateIdentity(10)) * norm(generateIdentity(10));
			copyToGui();
			result.setText("cond: " + to_string(cond));
		}
		
	});

	auto yakobiButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "yakobi");
	yakobiButton->setOnReleaseCallback([&result]()
	{
		copyFromGui();
		if(!copyErrorFlag)
		{
			auto res = yakobi(matrixValues);
			result.setText("yakobi applied");
			auto identity = generateIdentity(10);
			for(int i = 0; i < 10; i++)
			{
				for(int j = 0; j < 10; j++)
				{
					matrixValues[i][j] = identity[i][j];
				}
				matrixValues[i][10] = res[i];
			}
			copyToGui();
			
		}
	});

	auto zeidelButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "zeidel");
	zeidelButton->setOnReleaseCallback([&result]()
	{
		copyFromGui();
		if (!copyErrorFlag)
		{
			auto res = zeidel(matrixValues);
			result.setText("zeidel applied");
			auto identity = generateIdentity(10);
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					matrixValues[i][j] = identity[i][j];
				}
				matrixValues[i][10] = res[i];
			}
			copyToGui();

		}
	});

	auto descentButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "descent");
	descentButton->setOnReleaseCallback([&result]()
	{
		copyFromGui();
		if (!copyErrorFlag)
		{
			auto res = descent(matrixValues);
			result.setText("descent applied");
			auto identity = generateIdentity(10);
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					matrixValues[i][j] = identity[i][j];
				}
				matrixValues[i][10] = res[i];
			}
			copyToGui();

		}
	});

	getOffset();

	auto hilbertButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "gen hilbert");
	hilbertButton->setOnReleaseCallback([&result]()
	{
		generateHilbert(matrixValues);
		copyToGui();
	});

	auto randomButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "gen random");
	randomButton->setOnReleaseCallback([]()
	{
		generateRandom(matrixValues);
		copyToGui();
	});
	
	auto diagButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "gen diag");
	diagButton->setOnReleaseCallback([]()
	{
		generateDiag(matrixValues);
		copyToGui();
	});
	
	auto saveButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "save matrix");
	saveButton->setOnReleaseCallback([]()
	{
		//generateRandom(matrixValues);
		buffer = matrixValues;
		copyToGui();
	});
	
	auto restoreButton = createButton(elemX, -matrixSize.second / 2 + getOffset(), remainingWidth, CELL_SIZE, "restore matrix");
	restoreButton->setOnReleaseCallback([]()
	{
		//generateRandom(matrixValues);
		matrixValues = buffer;
		copyToGui();
	});


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