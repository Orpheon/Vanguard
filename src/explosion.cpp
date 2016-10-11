#include "explosion.h"
#include "renderer.h"

Explosion::Explosion(uint64_t id_, Gamestate *state, std::string animationpath) : MovingEntity(id_, state), explosionanim(animationpath, std::bind(&Explosion::suicide, this, state))
{
    //ctor
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
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->foreground);
    al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
}
