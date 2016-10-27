#include "ingameelements/projectiles/flashbang.h"
#include "renderer.h"
#include "ingameelements/explosion.h"

#include <functional>

Flashbang::Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_) : Projectile::Projectile(id_, state, owner_), countdown(std::bind(&Flashbang::explode, this, state), 0.5)
{
    //ctor
}

Flashbang::~Flashbang()
{
    //dtor
}

void Flashbang::midstep(Gamestate *state, double frametime)
{
    Projectile::midstep(state, frametime);
    countdown.update(state, frametime);
}

void Flashbang::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);

    double direction = std::atan2(vspeed, hspeed);
    al_set_target_bitmap(renderer->background);
    al_draw_rotated_bitmap(sprite, 0, 0, x - renderer->cam_x, y - renderer->cam_y, direction, 0);
}

void Flashbang::explode(Gamestate *state)
{
    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/flashbang_explosion/", 0));
    e->x = x;
    e->y = y;
    // TODO: Collide with nearby players
    destroy(state);
}
