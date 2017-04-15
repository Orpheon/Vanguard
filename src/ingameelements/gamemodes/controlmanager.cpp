#include "ingameelements/gamemodes/controlmanager.h"

void ControlManager::serialize(WriteBuffer &buffer, Gamestate &state)
{
	buffer.write<uint8_t>(teamscore[TEAM1]);
	buffer.write<uint8_t>(teamscore[TEAM2]);
	buffer.write<uint16_t>(teamcappingrate[TEAM1] * 65536 / 100.0);
	buffer.write<uint16_t>(teamcappingrate[TEAM2] * 65536 / 100.0);
	ControlPoint &cp = state.get<ControlPoint>(controlpoint);
	buffer.write<uint16_t>(cp.cappedamount[TEAM1] * 65536 / 100.0);
	buffer.write<uint16_t>(cp.cappedamount[TEAM2] * 65536 / 100.0);
}

void ControlManager::deserialize(ReadBuffer &buffer, Gamestate &state)
{
	teamscore[TEAM1] = buffer.read<uint8_t>();
	teamscore[TEAM2] = buffer.read<uint8_t>();
	teamcappingrate[TEAM1] = 100 * buffer.read<uint16_t>() / 65536;
	teamcappingrate[TEAM2] = 100 * buffer.read<uint16_t>() / 65536;
	ControlPoint &cp = state.get<ControlPoint>(controlpoint);
	cp.cappedamount[TEAM1] = 100 * buffer.read<uint16_t>() / 65536;
	cp.cappedamount[TEAM2] = 100 * buffer.read<uint16_t>() / 65536;
}
