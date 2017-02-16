#include "ingameelements/projectile.h"
#include "engine.h"

#include <cmath>

void Projectile::init(uint64_t id_, Gamestate *state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::PROJECTILE;
    owner = owner_;
    team = state->get<Player>(owner)->team;
}

void Projectile::midstep(Gamestate *state, double frametime)
{
    if (isrectangular())
    {
        if (state->currentmap->collides(x, y, getrect(), std::atan2(vspeed, hspeed)))
        {
            oncollision(state);
        }
        for (auto pptr : state->playerlist)
        {
            Player *p = state->get<Player>(pptr);
            if (p->team != team)
            {
                Character *c = p->getcharacter(state);
                if (c != 0)
                {
                    if (rectcollides(state, p->character, std::atan2(vspeed, hspeed)))
                    {
                        oncollision(state, c);
                        break;
                    }
                }
            }
        }
    }
}

bool Projectile::rectcollides(Gamestate *state, EntityPtr otherentity, double angle)
{
    MovingEntity *m = state->get<MovingEntity>(otherentity);
    Rect self = state->engine->maskloader.get_rect(getsprite(state, true)).offset(x, y);
    Rect other = state->engine->maskloader.get_rect(m->getsprite(state, true)).offset(m->x, m->y);

    double maxdist = std::max(std::hypot(self.w, self.h), std::hypot(other.w, other.h));
    if (std::hypot(self.x - other.x, self.y - other.y) <= maxdist)
    {
        // We're close enough that an actual collision might happen
        // Check the sprites
        ALLEGRO_BITMAP *selfsprite = state->engine->maskloader.requestsprite(getsprite(state, true));
        ALLEGRO_BITMAP *othersprite = state->engine->maskloader.requestsprite(m->getsprite(state, true));

        double cosa = std::cos(angle);
        double sina = std::sin(angle);

        double tmpx, tmpy;

        for (int i=0; i<self.w; ++i)
        {
            for (int j=0; j<self.h; ++j)
            {
                tmpx = self.x + cosa*(self.x-x+i) - sina*(self.y-y+j);
                tmpy = self.y + sina*(self.x-x+i) + cosa*(self.y-y+j);
                if (al_get_pixel(selfsprite, i, j).a != 0 and al_get_pixel(othersprite, tmpx - other.x, tmpy - other.y).a != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Projectile::circlecollides(Gamestate *state, EntityPtr otherentity, double radius)
{
    MovingEntity *m = state->get<MovingEntity>(otherentity);
    Rect other = state->engine->maskloader.get_rect(m->getsprite(state, true)).offset(m->x, m->y);

    if (std::hypot(x - other.x, y - other.y) <= radius + std::hypot(other.w, other.h))
    {
        // We're close enough that an actual collision might happen
        // Check the sprites
        double r2 = radius*radius;
        ALLEGRO_BITMAP *othersprite = state->engine->maskloader.requestsprite(m->getsprite(state, true));

        for (int i=-radius; i<radius; ++i)
        {
            for (int j=-radius; j<radius; ++j)
            {
                if (i*i+j*j <= r2 and al_get_pixel(othersprite, x+i - other.x, y+j - other.y).a != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void Projectile::oncollision(Gamestate *state, Character *c)
{
    // Collided with player
    c->damage(state, damage());
    destroy(state);
}

void Projectile::oncollision(Gamestate *state)
{
    // Collided with wallmask
    destroy(state);
}
