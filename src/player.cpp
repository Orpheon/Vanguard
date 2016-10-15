#include "player.h"
#include "gamestate.h"
#include "datastructures.h"
#include "ingameelements/character.h"
#include "ingameelements/heroes/mccree.h"

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

void Player::spawn(Gamestate *state)
{
    if (character != 0)
    {
        // We already have a character, error and respawn
        fprintf(stderr, "\nERROR: Tried to spawn character that was already alive.");
    }
    character = state->make_entity<Mccree>(state, EntityPtr(id));
    Character *c = state->get<Character>(character);
    c->x = 740;
    c->y = 356;
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

void Player::serialize(Gamestate *state, WriteBuffer *buffer)
{
    buffer->write<bool>(character != 0);
    if (character != 0)
    {
        Character *c = state->get<Character>(character);
        if (c->issynced())
        {
            c->serialize(state, buffer);
        }
    }
}

void Player::deserialize(Gamestate *state, ReadBuffer *buffer)
{
    bool havechar = buffer->read<bool>();
    if (havechar != (character != 0))
    {
        fprintf(stderr, "\nERROR: Character existence inconsistency while deserializing");
    }
    if (havechar)
    {
        Character *c = state->get<Character>(character);
        if (c->issynced())
        {
            c->deserialize(state, buffer);
        }
    }
}
