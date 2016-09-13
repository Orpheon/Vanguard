#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "player.h"
#include "datastructures.h"


class Mccree : public Character
{
    public:
        Mccree(Gamestate *state, PlayerPtr owner_);
        virtual ~Mccree();
        void render(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *midground, ALLEGRO_BITMAP *foreground, int cam_x, int cam_y);
    protected:
    private:
        std::string sprite = "sprites/heroes/mccree/mccree.png";
};

#endif // MCCREE_H
