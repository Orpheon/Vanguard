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

void Flashbang::midstep(Gamestate *state, double frametime)
{
    Projectile::midstep(state, frametime);
    countdown.update(state, frametime);
}

void Flashbang::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    double direction = std::atan2(vspeed, hspeed);
    al_set_target_bitmap(renderer->background);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, direction, 0);
}

void Flashbang::explode(Gamestate *state)
{
    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/flashbang_explosion/", 0));
    e->x = x;
    e->y = y;

    for (auto p : state->playerlist)
    {
        // DEBUGTOOL: Replace this check with checking whether p is on enemy team
        if (p != owner)
        {
            Character *c = state->get<Player>(p)->getcharacter(state);
            if (c != 0)
            {
                if (circlecollides(state, state->get<Player>(p)->character, EXPLOSION_RADIUS))
                {
                    // Check that they aren't behind a wall or something
                    if (not state->currentmap->collideline(x, y, c->x, c->y))
                    {
                        c->stunanim.reset();
                    }
                }
            }
        }
    }

    destroy(state);
}
