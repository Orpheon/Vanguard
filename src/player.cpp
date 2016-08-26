#include "player.h"
#include "gamestate.h"

#include "mccree.h"

Player::Player(Gamestate *state)
{
    state->playerlist.push_back(this);
    character = 0;
}

Player::~Player()
{
    delete character;
}

void Player::midstep(Gamestate *state, double frametime)
{
    ;
}

Player* Player::clone(Gamestate *state)
{
    Player *p = new Player(state);
    p->character = character;
    return p;
}

void Player::spawn(Gamestate *state, double x, double y)
{
    if (character != 0)
    {
        // We already have a character, error and respawn
        fprintf(stderr, "\nERROR: Tried to spawn character that was already alive.");
        delete character;
    }

    character = new Mccree(state, this);
    character->x = x;
    character->y = y;
}
