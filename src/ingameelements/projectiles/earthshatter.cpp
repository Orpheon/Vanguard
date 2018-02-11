
#include "ingameelements/projectiles/earthshatter.h"

#include "ingameelements/explosion.h"

void Earthshatter::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);
    owner = owner_;

    direction = state.get<Player&>(owner).getcharacter(state).isflipped ? -1 : 1;
    distance.init(MAX_RANGE, std::bind(&Earthshatter::destroy, this, std::placeholders::_1), true);
    olddistance = 0;
    explosionspawner.init(EXPLOSION_STEPSIZE, std::bind(&Earthshatter::spawnexplosion, this, std::placeholders::_1),
                          true);
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

    // Stop if collides with a shield
    Team myteam = state.get<Player&>(owner).team;
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        // If the entity blocks the stuff a shield would block
        if (entity.damageableby(myteam) and entity.blocks(penetrationlevel))
        {
            double enemycenterx=0, enemycentery=0;
            double dist = entity.maxdamageabledist(state, &enemycenterx, &enemycentery);
            if (std::hypot(enemycenterx-x, enemycentery-y) <= dist)
            {
                if (entity.collides(state, x, y))
                {
                    destroy(state);
                }
            }
        }
    }

    distance.update(state, frametime*SPEED);
}

void Earthshatter::endstep(Gamestate &state, double frametime)
{
    Team myteam = state.get<Player&>(owner).team;
    Team enemyteam = myteam == TEAM1 ? TEAM2 : TEAM1;

    // FIXME Resource hog, maybe increment could be larger than 1 without drawback? Or do distance checking beforehand
    for (int h=0; h<60; ++h)
    {
        if (state.currentmap->testpixel(x, y-h) or state.currentmap->spawnroom(state, enemyteam).isinside(x, y-h))
        {
            // We hit wallmask or went out of bounds or hit enemy spawnroom
            break;
        }
        for (auto &p : state.playerlist)
        {
            auto &player = state.get<Player&>(p);
            if (state.exists(player.character) and player.team == enemyteam)
            {
                auto &character = player.getcharacter(state);
                if (not character.earthshatteredfallanim.active() and not character.earthshatteredanim.active()
                    and not character.earthshatteredgetupanim.active())
                {
                    double enemycenterx=0, enemycentery=0;
                    double dist = character.maxdamageabledist(state, &enemycenterx, &enemycentery);
                    if (std::hypot(enemycenterx-x, enemycentery-y+h) <= dist)
                    {
                        if (character.collides(state, x, y-h))
                        {
                            // Check first if there's a shield protecting the character
                            if (state.collidelineshielded(x, y, x, y-h, character, myteam, penetrationlevel).id
                                == character.id)
                            {
                                double effectivedamage = character.damage(state, 50);
                                state.get<Player&>(owner).registerdamage(state, effectivedamage);
                                character.earthshatteredfallanim.reset();
                                character.interrupt(state);
                            }
                        }
                    }
                }
            }
        }
    }
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
