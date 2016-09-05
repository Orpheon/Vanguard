#include "player.h"
#include "gamestate.h"
#include "datastructures.h"

#include "mccree.h"

Player::Player(Gamestate *state)
{
    state->playerlist.push_back(this);
    characterid = 0;
}

Player::~Player(Gamestate *state)
{
    delete state->entitylist[characterid];
}

void Player::midstep(Gamestate *state, double frametime)
{
    ;
}

void Player::clone(Gamestate *oldstate, Gamestate *newstate)
{
    PlayerPtr p = newstate->make_player();
    newstate->get(p)->character = character;
}

void Player::spawn(Gamestate *state, double x, double y)
{
    if (character != 0)
    {
        // We already have a character, error and respawn
        fprintf(stderr, "\nERROR: Tried to spawn character that was already alive.");
        delete character;
    }
    character = state->make_entity<Mccree>(state, this);
    character->x = x;
    character->y = y;
}
