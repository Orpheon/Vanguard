#pragma once
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "gamestate.h"
#include "mapelements/controlpoint.h"

class ControlManager : public GameModeManager
{
public:
	virtual void serialize(WriteBuffer *buffer, Gamestate *state);
	virtual void deserialize(ReadBuffer *buffer, Gamestate *state);

	Gamemode gamemode = Gamemode::CONTROL;
	uint8_t teamscore[2] = { 0, 0 };
	double teamcappingrate[2] = { 0, 0 };
	EntityPtr controlpoint;
};