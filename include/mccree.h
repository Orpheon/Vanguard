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
        void render(Renderer *renderer);
        CharacterChildParameters constructCharacterChildParameters();
        Rect getcollisionrect(Gamestate *state);
        std::string getmask();
    protected:
    private:
};

#endif // MCCREE_H
