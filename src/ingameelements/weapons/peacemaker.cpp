#include <cmath>

#include "ingameelements/weapons/peacemaker.h"
#include "renderer.h"
#include "ingameelements/projectiles/peacemakerbullet.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/explosion.h"
#include "ingameelements/trail.h"
#include "engine.h"

Peacemaker::Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_) : Clipweapon(id_, state, owner_, constructparameters(state)),
                        fthanim("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::firesecondary, this, state)), isfthing(false),
                        deadeyetargets(), deadeyeanim("heroes/mccree/fanthehammerloop/"), isfiringult(false)
{
    fthanim.active(false);
    deadeyeanim.active(false);
}

Peacemaker::~Peacemaker()
{
    //dtor
}

void Peacemaker::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite;
    double dir = aimdirection;
    Mccree *c = state->get<Mccree>(state->get<Player>(owner)->character);
    if (firinganim.active())
    {
        mainsprite = firinganim.getframepath();
    }
    else if (reloadanim.active())
    {
        mainsprite = reloadanim.getframepath();
        dir = 3.1415*c->isflipped;
    }
    else if (fthanim.active())
    {
        mainsprite = fthanim.getframepath();
    }
    else
    {
        mainsprite = "heroes/"+c->getcharacterfolder()+"arm/1";
    }
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite)*renderer->zoom;
    double spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite)*renderer->zoom;
    double rel_x = (x - renderer->cam_x)*renderer->zoom;
    double rel_y = (y - renderer->cam_y)*renderer->zoom;

    al_set_target_bitmap(renderer->midground);
    if (c->weaponvisible(state))
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, getattachpoint_x()+spriteoffset_x, getattachpoint_y()+spriteoffset_y, rel_x, rel_y, 1, -1, dir, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, getattachpoint_x()+spriteoffset_x, getattachpoint_y()+spriteoffset_y, rel_x, rel_y, dir, 0);
        }
    }
}

void Peacemaker::midstep(Gamestate *state, double frametime)
{
    Clipweapon::midstep(state, frametime);

    if (isfthing)
    {
        fthanim.active(true);
    }
    fthanim.update(state, frametime);

    Player *ownerplayer = state->get<Player>(owner);
    Mccree *ownerchar = state->get<Mccree>(ownerplayer->character);
    if (ownerchar->ulting.active and not isfiringult)
    {
        for (auto p : state->playerlist)
        {
            Player *player = state->get<Player>(p);
            if (player->team != SPECTATOR and player->team != team)
            {
                Character *c = player->getcharacter(state);
                if (c != 0)
                {
                    if (not state->currentmap->collideline(x, y, c->x, c->y))
                    {
                        if (deadeyetargets.count(p) == 0)
                        {
                            deadeyetargets[p] = 0;
                        }
                        deadeyetargets[p] += frametime*170;
                    }
                }
            }
        }
    }
}

void Peacemaker::reload(Gamestate *state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active() and not isfthing)
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Peacemaker::wantfireprimary(Gamestate *state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and not isfthing and not isfiringult and state->engine->isserver)
    {
        fireprimary(state);
        state->engine->sendbuffer->write<uint8_t>(PRIMARY_FIRED);
        state->engine->sendbuffer->write<uint8_t>(state->findplayerid(owner));
    }
}

void Peacemaker::fireprimary(Gamestate *state)
{
    double cosa = std::cos(aimdirection), sina = std::sin(aimdirection);
    double collisionptx, collisionpty;
    double d = std::hypot(state->currentmap->width(), state->currentmap->height());
    EntityPtr target = state->collidelinedamageable(x, y, x+cosa*d, y+sina*d, team, &collisionptx, &collisionpty);
    if (target.id != 0)
    {
        double distance = std::hypot(collisionptx-x, collisionpty-y);
        double falloff = 1.0;
        if (distance > FALLOFF_BEGIN)
        {
            falloff = std::max(0.0, 1 - (distance-FALLOFF_BEGIN)/(FALLOFF_END-FALLOFF_BEGIN));
        }
        MovingEntity *m = state->get<MovingEntity>(target);
        if (m->entitytype == CHARACTER)
        {
            Character *c = reinterpret_cast<Character*>(m);
            c->damage(state, MAX_DAMAGE*falloff);
        }
    }

    state->make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection));
    e->x = x+cosa*24;
    e->y = y+sina*24;

    --clip;
    firinganim.reset();
    firinganim.active(true);
}

void Peacemaker::wantfiresecondary(Gamestate *state)
{
    if (clip > 0)
    {
        if (not isfthing and state->engine->isserver and not reloadanim.active() and not firinganim.active() and not isfiringult)
        {
            firesecondary(state);
            state->engine->sendbuffer->write<uint8_t>(SECONDARY_FIRED);
            state->engine->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
        else if (isfthing and fthanim.getpercent() >= 1)
        {
            firesecondary(state);
            state->engine->sendbuffer->write<uint8_t>(SECONDARY_FIRED);
            state->engine->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
    }
    else
    {
        isfthing = false;
        fthanim.active(false);
    }
}

void Peacemaker::firesecondary(Gamestate *state)
{
    double spread = (2*(rand()/(RAND_MAX+1.0)) - 1)*25*3.1415/180.0;
    double cosa = std::cos(aimdirection+spread), sina = std::sin(aimdirection+spread);
    double collisionptx, collisionpty;
    EntityPtr target = state->collidelinedamageable(x, y, x+cosa*FALLOFF_END, y+sina*FALLOFF_END, team, &collisionptx, &collisionpty);
    if (target.id != 0)
    {
        double distance = std::hypot(collisionptx-x, collisionpty-y);
        double falloff = 1.0;
        if (distance > FALLOFF_BEGIN)
        {
            falloff = std::max(0.0, (distance-FALLOFF_BEGIN) / (FALLOFF_END-FALLOFF_BEGIN));
        }
        MovingEntity *m = state->get<MovingEntity>(target);
        if (m->entitytype == CHARACTER)
        {
            Character *c = reinterpret_cast<Character*>(m);
            c->damage(state, MAX_FTH_DAMAGE*falloff);
        }
    }

    state->make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection+spread));
    e->x = x+cosa*24;
    e->y = y+sina*24;

    --clip;

    if (clip > 0 and state->engine->isserver)
    {
        if (isfthing)
        {
            fthanim = Animation("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::wantfiresecondary, this, state));
        }
        else
        {
            fthanim = Animation("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::wantfiresecondary, this, state));
            isfthing = true;
        }
    }
}

void Peacemaker::fireultimate(Gamestate *state)
{
    isfiringult = true;
    if (deadeyetargets.size() > 0)
    {
        EntityPtr playerptr = 0;
        double distance = VIEWPORT_WIDTH*10;
        Character *c;
        // Select closest target
        for (auto p : deadeyetargets)
        {
            Player *player = state->get<Player>(p.first);
            c = player->getcharacter(state);
            if (c != 0)
            {
                double d = std::hypot(c->x-x, c->y-y);
                if (d < distance)
                {
                    distance = d;
                    playerptr = p.first;
                }
            }
        }

        c = state->get<Player>(playerptr)->getcharacter(state);
        double collisionptx, collisionpty;
        EntityPtr target = state->collidelinedamageable(x, y, c->x, c->y, team, &collisionptx, &collisionpty);
        double angle = std::atan2(c->y-y, c->x-x), cosa = std::cos(angle), sina = std::sin(angle);
        MovingEntity *m = state->get<MovingEntity>(target);
        if (m != 0 and m->entitytype == CHARACTER)
        {
            c = reinterpret_cast<Character*>(m);
            c->damage(state, deadeyetargets[playerptr]);
        }
        state->make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
        Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", angle));
        e->x = x+cosa*24;
        e->y = y+sina*24;

        deadeyetargets.erase(playerptr);

        deadeyeanim = Animation("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::fireultimate, this, state));
    }
    else
    {
        Mccree *ownerchar = state->get<Mccree>(state->get<Player>(owner)->character);
        ownerchar->resetafterult(state);
    }
}

WeaponChildParameters Peacemaker::constructparameters(Gamestate *state)
{
    WeaponChildParameters p;
    p.clipsize = 6;
    p.characterfolder = "heroes/mccree/";
    p.reloadfunction = std::bind(&Peacemaker::restoreclip, this, state);
    return p;
}
