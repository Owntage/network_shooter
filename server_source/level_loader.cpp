#include "level_loader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <map>
#include <components/coord_event.h>
#include <components/string_event.h>

using boost::property_tree::ptree;

LevelLoader::LevelLoader(GameLogic& gameLogic):
	gameLogic(gameLogic) {}

bool LevelLoader::loadLevel(std::string path, std::string levelName)
{
	try
	{
		ptree level;
		read_xml(path + "/" + levelName, level);
		int width;
		int height;
		level = level.get_child("map");
		std::map<int, std::string> tiles;
		width = level.get<int>("<xmlattr>.width");
		height = level.get<int>("<xmlattr>.height");
		auto tileset = level.get_child("tileset");
		int offset = tileset.get<int>("<xmlattr>.firstgid");
		BOOST_FOREACH(auto& tilesetChild, tileset)
		{
			if(tilesetChild.first == "tile")
			{
				int id = tilesetChild.second.get<int>("<xmlattr>.id") + offset;
				tiles[id] = tilesetChild.second.get<std::string>("image.<xmlattr>.source");
			}
		}
		boost::property_tree::ptree layer = level.get_child("layer");
		int counter = 0;
		BOOST_FOREACH(auto& tile, layer.get_child("data"))
		{
			if(tile.second.get<int>("<xmlattr>.gid") != 0)
			{
				//backgrounds.push_back(new Background((double)(counter % width), (double)(counter / height), tiles[tile.second.get<int>("<xmlattr>.gid")], 0.0));
				int tileActor = gameLogic.createActor("tileActor");
				gameLogic.onEvent(CoordEvent("set_coords", tileActor, counter % width, counter / width));
				gameLogic.onEvent(StringEvent("set_image", tileActor, path + "/" + tiles[tile.second.get<int>("<xmlattr>.gid")]));
			}
			counter++;
		}

		//parsing objects

		return true;
	}
	catch(...)
	{
		return false;
	}
	
}

void LevelLoader::deleteAllLevels()
{
	for(int i = 0; i < objects.size(); i++)
	{
		gameLogic.destroyActor(objects[i]);
	}
}