#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "renderer.h"
#include "datastructures.h"


class Mccree : public Character
{
    public:
        Mccree(Gamestate *state, PlayerPtr owner_);
        virtual ~Mccree();
        void render(Renderer *renderer);
    protected:
    private:
};

#endif // MCCREE_H
