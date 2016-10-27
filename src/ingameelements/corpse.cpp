#include "corpse.h"

#include "gamestate.h"
#include "engine.h"
#include "renderer.h"

Corpse::Corpse(uint64_t id_, Gamestate *state, std::string characterfolder, bool isflipped_) : MovingEntity(id_, state), spritepath(characterfolder+"corpse/1"),
                countdown(std::bind(&Corpse::destroy, this, state), 8), isflipped(isflipped_)
{
    //ctor
}

Corpse::~Corpse()
{
    //dtor
}

void Corpse::midstep(Gamestate *state, double frametime)
{
    countdown.update(state, frametime);

    // Gravity
    vspeed += 540.0*frametime;

    // Collision with wallmask
    if (state->currentmap->collides(state->engine->maskloader.get_rect(getsprite(state, true))))
    {
        vspeed = 0;
    }
}

void Corpse::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->background);
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

void Corpse::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    Corpse *prev_e = static_cast<Corpse*>(prev_entity);
    Corpse *next_e = static_cast<Corpse*>(next_entity);

    countdown.timer = prev_e->countdown.timer + alpha*(next_e->countdown.timer - prev_e->countdown.timer);
}
