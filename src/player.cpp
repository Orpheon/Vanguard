#include "player.h"
#include "gamestate.h"
#include "datastructures.h"
#include "character.h"
#include "mccree.h"

Player::Player(uint64_t id_, Gamestate *state) : Entity(id_), character(0)
{
    ;
}

Player::~Player()
{
    ;
}

void Player::beginstep(Gamestate *state, double frametime)
{
    if (character != 0)
    {
        state->get<Character>(character)->beginstep(state, frametime);
    }
}

void Player::midstep(Gamestate *state, double frametime)
{
    if (character != 0)
    {
        state->get<Character>(character)->midstep(state, frametime);
    }
}

void Player::endstep(Gamestate *state, double frametime)
{
    if (character != 0)
    {
        state->get<Character>(character)->endstep(state, frametime);
    }
}

void Player::render(Renderer *renderer, Gamestate *state)
{
    if (character != 0)
    {
        state->get<Character>(character)->render(renderer, state);
    }
}

void Player::spawn(Gamestate *state, double x, double y)
{
    if (character != 0)
    {
        // We already have a character, error and respawn
        fprintf(stderr, "\nERROR: Tried to spawn character that was already alive.");
    }
    character = state->make_entity<Mccree>(state, EntityPtr(id));
    Character *c = static_cast<Character*>(state->get<Character>(character));
    c->x = x;
    c->y = y;
}

Character* Player::getcharacter(Gamestate *state)
{
    return state->get<Character>(character);
}

void Player::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    Player *prev_e = static_cast<Player*>(prev_entity);
    Player *next_e = static_cast<Player*>(next_entity);

    if (alpha < 0.5)
    {
        character = prev_e->character;
    }
    else
    {
        character = next_e->character;
    }
}
