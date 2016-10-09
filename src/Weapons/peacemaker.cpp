#include "peacemaker.h"

Peacemaker::Peacemaker(Gamestate *state, EntityPtr owner_) : Weapon(state, owner_)
{

}

Peacemaker::~Peacemaker()
{
    //dtor
}

Peacemaker::render(Renderer *renderer, Gamestate *state)
{
    // FIXME: Placeholder
    std::string mainsprite = "heroes/mccree/arm/1.png";
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    al_draw_rotated_bitmap(sprite, xoffset, yoffset, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, aimdirection, 0);
}
