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
        std::string sprite = "sprites/heroes/mccree/mccree.png";
        std::string mask = "masks/heroes/mccree/mccree.png";
};

#endif // MCCREE_H
