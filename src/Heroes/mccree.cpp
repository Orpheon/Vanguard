#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"

#include <memory>

Mccree::Mccree(Gamestate *state, PlayerPtr owner_) : Character(state, owner_)
{
    mask = "heroes/mccree/mccree.png";
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer)
{
    ALLEGRO_BITMAP *sprite =renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
    }
}
