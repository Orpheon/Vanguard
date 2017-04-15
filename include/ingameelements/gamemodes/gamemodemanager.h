#pragma once
#include "datastructures.h"

class GameModeManager
{
public:
	virtual void serialize(WriteBuffer &buffer, Gamestate &state){};
	virtual void deserialize(ReadBuffer &buffer, Gamestate &state){};
	Gamemode gamemode;
	EntityPtr spawnrooms[2];
};