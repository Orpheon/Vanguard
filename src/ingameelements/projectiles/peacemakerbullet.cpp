#include <cmath>

#include "ingameelements/projectiles/peacemakerbullet.h"
#include "renderer.h"

void PeacemakerBullet::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite)*renderer->zoom;
    double spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite)*renderer->zoom;
    double rel_x = (x - renderer->cam_x)*renderer->zoom;
    double rel_y = (y - renderer->cam_y)*renderer->zoom;
    double direction = std::atan2(vspeed, hspeed);

    al_set_target_bitmap(renderer->background);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, direction, 0);
}
