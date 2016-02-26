#ifndef MOVE_EVENT_H
#define MOVE_EVENT_H

#include "../event.h"

struct MoveEvent : Event
{
	MoveEvent(bool left, bool right, bool up, bool down, int actorID): Event("move", false, actorID), left(left), right(right), up(up), down(down) {} 
	bool left, right, up, down;
	bool operator==(const MoveEvent& other)
	{
		return left == other.left && right == other.right && up == other.up && down == other.down;
	}
};

#endif