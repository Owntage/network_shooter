#ifndef TILE_UPDATE_H
#define TILE_UPDATE_H

#include "component_update.h"

struct TileUpdate : ComponentUpdate
{
	TileUpdate(std::string image, float x, float y) : ComponentUpdate("tile"),
		image(image), x(x), y(y) {}
	TileUpdate() {}
	std::string image;
	float x;
	float y;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, TileUpdate& u)
	{
		s << u.image << u.x << u.y;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, TileUpdate& u)
	{
		s >> u.image >> u.x >> u.y;
		return s;
	}
};

#endif