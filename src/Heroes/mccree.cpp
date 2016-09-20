#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"

#include <memory>

Mccree::Mccree(Gamestate *state, PlayerPtr owner_) : Character(state, owner_)
{
    sprite = "sprites/heroes/mccree/mccree.png";
    mask = "masks/heroes/mccree/mccree.png";
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer)
{
    al_set_target_bitmap(renderer->midground);
    if (mouse_x < x-renderer->cam_x+25)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(renderer->spriteloader.request_sprite(sprite), 44, 0, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(renderer->spriteloader.request_sprite(sprite), x-renderer->cam_x, y-renderer->cam_y, 0);
    }
}
