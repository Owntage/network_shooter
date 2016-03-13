#ifndef MOVE_UPDATE_H
#define MOVE_UPDATE_H

#include "component_update.h"

struct MoveUpdate : ComponentUpdate
{
	MoveUpdate(float x, float y) : ComponentUpdate("move"), x(x), y(y) {}
	MoveUpdate() : ComponentUpdate("move") {}
	float x;
	float y;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, MoveUpdate& moveUpdate)
	{
		s << moveUpdate.x << moveUpdate.y;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, MoveUpdate& moveUpdate)
	{
		s >> moveUpdate.x >> moveUpdate.y;
		return s;
	}
};

#endif