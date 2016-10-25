#include "ingameelements/explosion.h"
#include "renderer.h"

Explosion::Explosion(uint64_t id_, Gamestate *state, std::string animationpath, double direction_) : MovingEntity(id_, state),
                        explosionanim(animationpath, std::bind(&Explosion::destroy, this, state)), direction(direction_)
{
    entitytype = EXPLOSION;
}

Explosion::~Explosion()
{
    //dtor
}

void Explosion::midstep(Gamestate *state, double frametime)
{
    explosionanim.update(state, frametime);
}

void Explosion::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);
    Rect spritesize = renderer->spriteloader.get_rect(mainsprite);

    al_set_target_bitmap(renderer->foreground);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, direction, 0);
}
