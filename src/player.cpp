#include "player.h"
#include "gamestate.h"
#include "datastructures.h"

#include "mccree.h"

Player::Player(Gamestate *state) : character(0)
{
    ;
}

Player::~Player()
{
    ;
}

void Player::midstep(Gamestate *state, double frametime)
{
    ;
}

void Player::spawn(Gamestate *state, double x, double y)
{
    if (character != 0)
    {
        // We already have a character, error and respawn
        fprintf(stderr, "\nERROR: Tried to spawn character that was already alive.");
    }
    character = state->make_entity<Mccree>(state, PlayerPtr(id));
    Character *c = static_cast<Character*>(state->get(character));
    c->x = x;
    c->y = y;
}
