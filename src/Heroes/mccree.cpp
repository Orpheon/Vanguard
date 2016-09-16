#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"

#include <memory>

Mccree::Mccree(Gamestate *state, PlayerPtr owner_) : Character(state, owner_)
{
    // Constructor for normal constuction
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer)
{
    al_set_target_bitmap(renderer->midground);
    al_draw_bitmap(renderer->spriteloader.request_sprite(sprite), x-renderer->cam_x, y-renderer->cam_y, 0);
}
