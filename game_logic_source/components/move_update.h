#ifndef MOVE_UPDATE_H
#define MOVE_UPDATE_H

#include "component_update.h"

struct MoveUpdate : ComponentUpdate
{
	MoveUpdate(float x, float y, float sizeX, float sizeY) :
		ComponentUpdate("move"),
		x(x),
		y(y),
		speedX(0.0f),
		speedY(0.0f),
		time(0.0f),
		sizeX(sizeX),
		sizeY(sizeY)
	{}

	MoveUpdate(float x, float y, float speedX, float speedY, float time) :
		ComponentUpdate("move"), 
		x(x), 
		y(y),
		speedX(speedX),
		speedY(speedY),
		time(time),
		sizeX(0.0f),
		sizeY(0.0f)
	{}
	MoveUpdate(float x, float y) : 
		x(x),
		y(y),
		speedX(0.0f),
		speedY(0.0f),
		time(0.0f),
		sizeX(0.0f),
		sizeY(0.0f)
	{}
	MoveUpdate() : ComponentUpdate("move") {}
	float x;
	float y;
	float speedX;
	float speedY;
	float sizeX;
	float sizeY; //todo: create shape update to describe shape
	float time;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, MoveUpdate& moveUpdate)
	{
		s << moveUpdate.x << moveUpdate.y << moveUpdate.speedX << moveUpdate.speedY << moveUpdate.time
				<< moveUpdate.sizeX << moveUpdate.sizeY;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, MoveUpdate& moveUpdate)
	{
		s >> moveUpdate.x >> moveUpdate.y >> moveUpdate.speedX >> moveUpdate.speedY >> moveUpdate.time
				>> moveUpdate.sizeX >> moveUpdate.sizeY;
		return s;
	}
};

#endif