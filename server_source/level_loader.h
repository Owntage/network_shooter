#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <game_logic.h>
#include <string>
#include <vector>

struct LevelLoader
{
	LevelLoader(GameLogic& gameLogic);
	bool loadLevel(std::string path, std::string levelName);
	void deleteAllLevels();
private:
	std::vector<int> objects;
	GameLogic& gameLogic;
};


#endif