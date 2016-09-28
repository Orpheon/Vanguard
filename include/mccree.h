#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "renderer.h"
#include "datastructures.h"

#include <string>

class Mccree : public Character
{
    public:
        Mccree(Gamestate *state, PlayerPtr owner_);
        virtual ~Mccree();
        void render(Renderer *renderer, Gamestate *state);
        CharacterChildParameters constructCharacterChildParameters();
        Rect getcollisionrect(Gamestate *state);
        Rect getstandingcollisionrect(Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask);
    protected:
    private:
};

#endif // MCCREE_H
