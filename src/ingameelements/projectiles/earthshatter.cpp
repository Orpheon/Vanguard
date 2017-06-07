
#include "ingameelements/projectiles/earthshatter.h"

#include "ingameelements/explosion.h"

void Earthshatter::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);
    owner = owner_;

    direction = state.get<Player&>(owner).getcharacter(state).isflipped ? -1 : 1;
    distance.init(MAX_RANGE, std::bind(&Earthshatter::destroy, this, std::placeholders::_1));
    olddistance = 0;
    explosionspawner.init(EXPLOSION_STEPSIZE, std::bind(&Earthshatter::spawnexplosion, this, std::placeholders::_1));
    // We want an explosion to happen immediately
    explosionspawner.timer = explosionspawner.duration;

    while (state.currentmap->testpixel(x, y))
    {
        --y;
    }
}

void Earthshatter::beginstep(Gamestate &state, double frametime)
{
    while (std::abs(distance.timer - olddistance) >= 1)
    {
        if (not explosionspawner.active)
        {
            // I can't do this in the callback because of timer structure
            explosionspawner.reset();
        }
        explosionspawner.update(state, 1);
        ++olddistance;

        x += direction;
        if (state.currentmap->testpixel(x, y))
        {
            // STAIRCASE_STEPSIZE is negative
            y += STAIRCASE_STEPSIZE;
            if (state.currentmap->testpixel(x, y))
            {
                destroy(state);
            }
        }
        else if (not state.currentmap->testpixel(x, y+1))
        {
            bool fixed = false;
            for (double tmpy = y; tmpy < y-STAIRCASE_STEPSIZE + 1; ++tmpy)
            {
                if (not state.currentmap->testpixel(x, tmpy) and state.currentmap->testpixel(x, tmpy+1))
                {
                    y = tmpy;
                    fixed = true;
                }
            }
            if (not fixed)
            {
                destroy(state);
            }
        }
    }
    distance.update(state, frametime*SPEED);
}

void Earthshatter::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Earthshatter &prev_e = static_cast<Earthshatter&>(prev_entity);
    Earthshatter &next_e = static_cast<Earthshatter&>(next_entity);

    distance.interpolate(prev_e.distance, next_e.distance, alpha);
    if (direction == 1)
    {
        olddistance = static_cast<int>(std::floor(distance.timer));
    }
    else
    {
        olddistance = static_cast<int>(std::ceil(distance.timer));
    }

    explosionspawner.interpolate(prev_e.explosionspawner, next_e.explosionspawner, alpha);
}

void Earthshatter::spawnexplosion(Gamestate &state)
{
    EntityPtr plumeptr = state.make_entity<Explosion>(state, "heroes/reinhardt/ult/plume/", 0);
    Explosion &plume = state.get<Explosion&>(plumeptr);
    plume.x = x;
    plume.y = y;
}
