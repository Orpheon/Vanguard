#include "ingameelements/movingentity.h"
#include "gamestate.h"
#include "entity.h"
#include "engine.h"

#include <cmath>

MovingEntity::MovingEntity(uint64_t id_, Gamestate *state) : Entity(id_)
{
    x = 0;
    y = 0;
    hspeed = 0;
    vspeed = 0;
}

void MovingEntity::endstep(Gamestate *state, double frametime)
{
    x += hspeed*frametime;
    y += vspeed*frametime;
}

void MovingEntity::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    MovingEntity *prev_e = static_cast<MovingEntity*>(prev_entity);
    MovingEntity *next_e = static_cast<MovingEntity*>(next_entity);

    x = prev_e->x + alpha*(next_e->x - prev_e->x);
    y = prev_e->y + alpha*(next_e->y - prev_e->y);
    hspeed = prev_e->hspeed + alpha*(next_e->hspeed - prev_e->hspeed);
    vspeed = prev_e->vspeed + alpha*(next_e->vspeed - prev_e->vspeed);
}

bool MovingEntity::collides(Gamestate *state, EntityPtr otherentity)
{
    MovingEntity *m = state->get<MovingEntity>(otherentity);
    Rect self = state->engine->maskloader.get_rect(getsprite(state, true)).offset(x, y);
    Rect other = state->engine->maskloader.get_rect(m->getsprite(state, true)).offset(m->x, m->y);
    Rect overlap = Rect(std::max(self.x, other.x), std::max(self.y, other.y), 0, 0);
    overlap.w = std::min(self.x+self.w, other.x+other.w) - overlap.x;
    overlap.h = std::min(self.y+self.h, other.y+other.h) - overlap.y;
    if (overlap.w > 0 and overlap.h > 0)
    {
        // There is an actual overlap of sprites, time to go pixel perfect
        ALLEGRO_BITMAP *selfsprite = state->engine->maskloader.request_sprite(getsprite(state, true));
        ALLEGRO_BITMAP *othersprite = state->engine->maskloader.request_sprite(m->getsprite(state, true));
        int selfdx = overlap.x - self.x;
        int selfdy = overlap.y - self.y;
        int otherdx = overlap.x - other.x;
        int otherdy = overlap.y - other.y;
        for (int i=0; i<overlap.w; ++i)
        {
            for (int j=0; j<overlap.h; ++j)
            {
                if (al_get_pixel(selfsprite, i+selfdx, j+selfdy).a != 0.0 and al_get_pixel(othersprite, i+otherdx, j+otherdy).a != 0.0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void MovingEntity::serialize(Gamestate *state, WriteBuffer *buffer)
{
    buffer->write<float>(x);
    buffer->write<float>(y);
    buffer->write<float>(hspeed);
    buffer->write<float>(vspeed);
}

void MovingEntity::deserialize(Gamestate *state, ReadBuffer *buffer)
{
    x = buffer->read<float>();
    y = buffer->read<float>();
    hspeed = buffer->read<float>();
    vspeed = buffer->read<float>();
}
