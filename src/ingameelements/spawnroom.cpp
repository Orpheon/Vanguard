#include "ingameelements/spawnroom.h"
#include "gamestate.h"
#include "renderer.h"

#include "allegro5/allegro_primitives.h"

Spawnroom::Spawnroom(uint64_t id_, Rect area_, Team team_) : Entity(id_), area(area_), team(team_)
{
    //ctor
}

Spawnroom::~Spawnroom()
{
    //dtor
}

void Spawnroom::beginstep(Gamestate *state, double frametime)
{
    for (auto pptr : state->playerlist)
    {
        Player *p = state->get<Player>(pptr);
        if (p->team == team)
        {
            Character *c = p->getcharacter(state);
            if (c != 0 and isinside(c->x, c->y))
            {
                c->hp = c->getmaxhp();
            }
        }
    }
}

void Spawnroom::render(Renderer *renderer, Gamestate *state)
{
    al_set_target_bitmap(renderer->background);
    int border = 3;
    al_draw_rectangle(area.x+border/2.0 - renderer->cam_x, area.y+border/2.0 - renderer->cam_y,
                      area.x+area.w-border/2.0 - renderer->cam_x, area.y+area.h-border/2.0 - renderer->cam_y, al_premul_rgba(248, 222, 0, 200), border);
    border = 10;
    al_draw_rectangle(area.x+border/2.0 - renderer->cam_x, area.y+border/2.0 - renderer->cam_y,
                      area.x+area.w-border/2.0 - renderer->cam_x, area.y+area.h-border/2.0 - renderer->cam_y, al_premul_rgba(248, 222, 0, 50), border);
}
