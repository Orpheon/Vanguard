#include "ingameelements/trail.h"

#include <allegro5/allegro_primitives.h>

Trail::Trail(uint64_t id_, Gamestate *state, ALLEGRO_COLOR color_, double x1_, double y1_, double x2_, double y2_, double duration) : Entity(id_),
                color(color_), x1(x1_), y1(y1_), x2(x2_), y2(y2_), countdown(std::bind(&Trail::destroy, this, state), duration)
{
    //ctor
}

Trail::~Trail()
{
    //dtor
}

void Trail::midstep(Gamestate *state, double frametime)
{
    countdown.update(state, frametime);
}

void Trail::render(Renderer *renderer, Gamestate *state)
{
    al_set_target_bitmap(renderer->background);
    al_draw_line(x1-renderer->cam_x, y1-renderer->cam_y, x2-renderer->cam_x, y2-renderer->cam_y, color, 1);
}

void Trail::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    Trail *prev_e = reinterpret_cast<Trail*>(prev_entity);
    Trail *next_e = reinterpret_cast<Trail*>(next_entity);

    countdown.interpolate(&(next_e->countdown), &(prev_e->countdown), alpha);
}
