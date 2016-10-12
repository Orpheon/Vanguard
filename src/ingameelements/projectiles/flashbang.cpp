#include "ingameelements/projectiles/flashbang.h"
#include "renderer.h"
#include "ingameelements/explosion.h"

#include <functional>

Flashbang::Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_) : Projectile::Projectile(id_, state, owner_), countdown(std::bind(&Flashbang::explode, this, state), 0.3)
{
    //ctor
}

Flashbang::~Flashbang()
{
    //dtor
}

void Flashbang::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);
    // Check collision with wallmask
    if (state->currentmap->collides(state, this))
    {
        explode(state);
    }
    countdown.update(state, frametime);
}

void Flashbang::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->background);
    al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
}

void Flashbang::explode(Gamestate *state)
{
    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/flashbang_explosion/"));
    e->x = x;
    e->y = y;
    destroyentity = true;
}
