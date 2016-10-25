#include "ingameelements/projectile.h"
#include "engine.h"

#include <cmath>

Projectile::Projectile(double id_, Gamestate *state, EntityPtr owner_) : MovingEntity(id_, state), owner(owner_)
{
    entitytype = PROJECTILE;
}

Projectile::~Projectile()
{
    //dtor
}

void Projectile::midstep(Gamestate *state, double frametime)
{
    if (isrectangular())
    {
        if (state->currentmap->collides(getrect(), std::atan2(vspeed, hspeed)))
        {
            destroy(state);
        }
        for (auto p : state->playerlist)
        {
            // DEBUGTOOL: Replace this check with checking whether p is on enemy team
            if (p != owner)
            {
                Character *c = state->get<Player>(p)->getcharacter(state);
                if (c != 0)
                {
                    if (collides(state, state->get<Player>(p)->character, std::atan2(vspeed, hspeed)))
                    {
                        oncollision(state, c);
                    }
                }
            }
        }
    }
}

bool Projectile::collides(Gamestate *state, EntityPtr otherentity, double angle)
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
                tmpx = self.x + cosa*i - sina*j;
                tmpy = self.y + sina*i + cosa*j;
                if (al_get_pixel(selfsprite, i, j).a != 0 and al_get_pixel(othersprite, tmpx - other.x, tmpy - other.y).a != 0)
                {
                    return true;
                }
            }
        }
        return false;
    }
}

void Projectile::oncollision(Gamestate *state, Character *c)
{
    c->damage(state, damage());
    destroy(state);
}
