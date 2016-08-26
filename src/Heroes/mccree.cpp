#include "mccree.h"

Mccree::Mccree(Gamestate *state) : Character(state)
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
    Mccree* m = new Mccree(state);
    Character::clonedata(m);
    return m;
}
