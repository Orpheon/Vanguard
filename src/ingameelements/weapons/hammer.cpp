#include <cmath>

#include "ingameelements/weapons/hammer.h"
#include "renderer.h"
#include "ingameelements/heroes/reinhardt.h"
#include "ingameelements/projectiles/firestrike.h"

#include "engine.h"

void Hammer::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Weapon::init(id_, state, owner_);

    barrierptr = state.make_entity<ReinhardtShield>(state, team, owner_);

    firestrikeanim.init(herofolder()+"firestrikebackarm/", false);
    firestrikedelay.init(firestrikeanim.timer.duration * 0.5,
                         std::bind(&Hammer::createfirestrike, this, std::placeholders::_1), false);
    firestrikecooldown.init(6, false);

    // 6th and 14th frame of an assumed 20 frame animation - ugly but idk better way
    firingdelay1.init(firinganim.timer.duration * 6.0/20.0, std::bind(&Hammer::hitarea, this, std::placeholders::_1),
                      false);
    firingdelay2.init(firinganim.timer.duration * 14.0/20.0, std::bind(&Hammer::hitarea, this, std::placeholders::_1),
                      false);
}

void Hammer::renderbehind(Renderer &renderer, Gamestate &state)
{
    if (firinganim.active())
    {
        return;
    }

    std::string mainsprite;
    Reinhardt &c = state.get<Reinhardt>(state.get<Player>(owner).character);
    if (firestrikeanim.active())
    {
        mainsprite = firestrikeanim.getframepath();
    }
    else if (barrier(state).active)
    {
        mainsprite = herofolder()+"shield/back";

    }
    else
    {
        mainsprite = herofolder()+"arm/back";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getbackattachpoint_x(state)*renderer.zoom;
    double attachpt_y = getbackattachpoint_y(state)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y,
                                          -1, 1, (aimdirection+3.1415)*barrier(state).active, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y,
                                                    -1, 1, (aimdirection+3.1415)*barrier(state).active, 0);
            }
        }
        else
        {
            al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y, aimdirection*barrier(state).active, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, aimdirection*barrier(state).active, 0);
            }
        }
    }
}

void Hammer::render(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    Reinhardt &c = state.get<Reinhardt>(state.get<Player>(owner).character);
    if (firinganim.active())
    {
        mainsprite = firinganim.getframepath();
    }
    else if (firestrikeanim.active())
    {
        mainsprite = herofolder()+"firestrikefrontarm/"+std::to_string(firestrikeanim.getframe());
    }
    else if (barrier(state).active)
    {
        mainsprite = herofolder()+"shield/front";

    }
    else
    {
        mainsprite = herofolder()+"arm/front";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getattachpoint_x(state)*renderer.zoom;
    double attachpt_y = getattachpoint_y(state)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, -attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            }
        }
        else
        {
            al_draw_bitmap(sprite, rel_x - (attachpt_x+spriteoffset_x), rel_y - (attachpt_y+spriteoffset_y), 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_bitmap(outline, outlinecolor, rel_x - (attachpt_x+spriteoffset_x), rel_y - (attachpt_y+spriteoffset_y), 0);
            }
        }
    }

    barrier(state).render(renderer, state);
}

void Hammer::beginstep(Gamestate &state, double frametime)
{
    Weapon::beginstep(state, frametime);
    barrier(state).beginstep(state, frametime);
    firestrikeanim.update(state, frametime);
    firestrikedelay.update(state, frametime);
    firestrikecooldown.update(state, frametime);
    firingdelay1.update(state, frametime);
    firingdelay2.update(state, frametime);
}

void Hammer::midstep(Gamestate &state, double frametime)
{
    Weapon::midstep(state, frametime);
    barrier(state).midstep(state, frametime);
}

void Hammer::endstep(Gamestate &state, double frametime)
{
    Weapon::endstep(state, frametime);
    barrier(state).endstep(state, frametime);
}

void Hammer::wantfireprimary(Gamestate &state)
{
    if (state.engine.isserver and not firinganim.active() and not firestrikeanim.active())
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Hammer::fireprimary(Gamestate &state)
{
    firinganim.reset();
    firinganim.active(true);
    firingdelay1.reset();
    firingdelay1.active = true;
    firingdelay2.reset();
    firingdelay2.active = true;
}

void Hammer::wantfiresecondary(Gamestate &state)
{
    // Do nothing
}

void Hammer::firesecondary(Gamestate &state)
{
    // Do nothing
}

void Hammer::createfirestrike(Gamestate &state)
{
    Firestrike &firestrike = state.get<Firestrike&>(state.make_entity<Firestrike>(state, owner));
    firestrike.x = x + std::cos(aimdirection) * 40;
    firestrike.y = y + std::sin(aimdirection) * 40;
    firestrike.hspeed = firestrike.SPEED * std::cos(aimdirection);
    firestrike.vspeed = firestrike.SPEED * std::sin(aimdirection);

    if (state.currentmap->collideline(x, y, firestrike.x, firestrike.y))
    {
        firestrike.destroy(state);
    }

    firestrikecooldown.reset();
}

void Hammer::hitarea(Gamestate &state)
{
    Reinhardt &reinhardt = state.get<Reinhardt>(state.get<Player>(owner).character);
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        if (not entity.destroyentity)
        {
            if (entity.damageableby(team))
            {
                int direction = (reinhardt.isflipped ? -1 : 1);
                bool foundcollision = false;
                for (int i=0; i<110 and not foundcollision; ++i)
                {
                    for (int j=0; j<60 and not foundcollision; ++j)
                    {
                        if (entity.collides(state, x + direction*(-20 + i), y - 30 + j))
                        {
                            // We hit something, check if it's protected
                            if (state.collidelineshielded(state, x, y, x + direction*(-20 + i), y - 30 + j,
                                                          state.get<MovingEntity&>(entity.id), team,
                                                          PENETRATE_CHARACTER).id == entity.id)
                            {
                                entity.damage(state, DAMAGE);
                            }
                            foundcollision = true;
                        }
                    }
                }
            }
        }
    }
}

double Hammer::getattachpoint_x(Gamestate &state)
{
    return 0;
}

double Hammer::getattachpoint_y(Gamestate &state)
{
    return 8;
}

double Hammer::getbackattachpoint_x(Gamestate &state)
{
    if (barrier(state).active)
    {
        return 6 * (state.get<Player&>(owner).getcharacter(state).isflipped ? 1:-1);
    }
    else
    {
        return 0;
    }
}

double Hammer::getbackattachpoint_y(Gamestate &state)
{
    if (barrier(state).active)
    {
        return 10;
    }
    else
    {
        return 8;
    }
}

ReinhardtShield& Hammer::barrier(Gamestate &state)
{
    return state.get<ReinhardtShield>(barrierptr);
}

void Hammer::destroy(Gamestate &state)
{
    barrier(state).destroy(state);
    Weapon::destroy(state);
}

void Hammer::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Weapon::interpolate(prev_entity, next_entity, alpha);

    Hammer &prev_e = static_cast<Hammer&>(prev_entity);
    Hammer &next_e = static_cast<Hammer&>(next_entity);

    firestrikeanim.interpolate(prev_e.firestrikeanim, next_e.firestrikeanim, alpha);
    firestrikedelay.interpolate(prev_e.firestrikedelay, next_e.firestrikedelay, alpha);
    firestrikecooldown.interpolate(prev_e.firestrikecooldown, next_e.firestrikecooldown, alpha);
    firinganim.interpolate(prev_e.firinganim, next_e.firinganim, alpha);
    firingdelay1.interpolate(prev_e.firingdelay1, next_e.firingdelay1, alpha);
    firingdelay2.interpolate(prev_e.firingdelay2, next_e.firingdelay2, alpha);
}