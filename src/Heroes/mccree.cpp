#include "mccree.h"
#include "player.h"

#include <memory>

Mccree::Mccree(Gamestate *state, PlayerPtr owner_) : Character(state, owner_)
{
    // Load mccree sprites
    sprite = al_load_bitmap("sprites/heroes/mccree/mccree.png");
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *midground, ALLEGRO_BITMAP *foreground, int cam_x, int cam_y)
{
    al_set_target_bitmap(midground);
//    al_draw_bitmap(sprite, x-cam_x, y-cam_y, 0);
}

void Mccree::clone(Gamestate *state)
{
    // TODO
    ;
}
