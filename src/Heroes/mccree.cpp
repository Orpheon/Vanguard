#include "mccree.h"
#include "player.h"

Mccree::Mccree(Gamestate *state, Player *owner) : Character(state, owner)
{
    //ctor
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render()
{

}

Mccree* Mccree::clone(Gamestate *state)
{
    Mccree* m = new Mccree(state, owner);
    Character::clonedata(m);
    return m;
}
