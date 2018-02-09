#include <vector>
#include <cmath>
#include <allegro5/allegro_primitives.h>

#include "ingameelements/character.h"
#include "gamestate.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/player.h"
#include "global_constants.h"
#include "engine.h"
#include "ingameelements/weapon.h"
#include "ingameelements/corpse.h"
#include "renderer.h"
#include "ingameelements/heroes/mccree.h"
#include "colorpalette.h"

void Character::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::CHARACTER;
    owner = owner_;
    weapon = constructweapon(state);
    hp = initializehealth();
    team = state.get<Player>(owner).team;
    runanim.init(herofolder()+"run/", true);
    crouchanim.init(herofolder()+"crouchwalk/", false);
    stunanim.init(herofolder()+"stun/", false);

    earthshatteredfallanim.init(herofolder()+"fallasleep/", std::bind(&Character::earthshatteredhitground, this,
                                                                      std::placeholders::_1), false);
    earthshatteredanim.init(herofolder()+"sleep/", std::bind(&Character::earthshatteredgetup, this,
                                                             std::placeholders::_1), false);
    earthshatteredgetupanim.init(herofolder()+"getup/", false);

    pinanim.init(herofolder()+"pinned/", false);
    ongroundsmooth.init(0.05, true);

    xblockedsmooth.init(0.02, false);
    yblockedsmooth.init(0.02, false);
    jumpcooldown.init(0.5, false);
    isflipped = false;
    acceleration = 300;
    // friction factor per second of null movement; calculated directly from Gang Garrison 2
    // from pygg2
    friction = 0.01510305449388463132584804061124;
}

void Character::setinput(Gamestate &state, InputContainer heldkeys_, double mouse_x_, double mouse_y_)
{
    heldkeys = heldkeys_;
    mouse_x = mouse_x_;
    mouse_y = mouse_y_;
    getweapon(state).setaim(mouse_x, mouse_y);
}

void Character::beginstep(Gamestate &state, double frametime)
{
    Player &ownerplayer = state.get<Player>(owner);

    if (cangetinput(state))
    {
        if (heldkeys.LEFT)
        {
            hspeed = std::max(hspeed - acceleration * runpower() * frametime, -maxhspeed(state));
        }
        if (heldkeys.RIGHT)
        {
            hspeed = std::min(hspeed + acceleration * runpower() * frametime, maxhspeed(state));
        }

        if (heldkeys.JUMP)
        {
            if (canjump(state))
            {
                vspeed = -250.0;
                jumpcooldown.reset();
            }
        }
        if (heldkeys.CROUCH)
        {
            if (not crouchanim.active())
            {
                crouchanim.active(true);
                crouchanim.reset();
            }
        }
        else if (crouchanim.active())
        {
            // We're crouched and we'd like to uncrouch
            // Do so only if we have the room
            if (not state.currentmap->collides(getstandingcollisionrect(state)))
            {
                crouchanim.active(false);
            }
        }

        if (heldkeys.RELOAD)
        {
            if (getweapon(state).hasclip())
            {
                state.get<Clipweapon>(weapon).reload(state);
            }
        }
        // Shooting
        if (heldkeys.PRIMARY_FIRE and canuseweapons(state))
        {
            getweapon(state).wantfireprimary(state);
        }
        if (heldkeys.SECONDARY_FIRE and canuseweapons(state))
        {
            getweapon(state).wantfiresecondary(state);
        }

        // Ulting
        if (heldkeys.ULTIMATE and not ownerplayer.ultcharge.active and canuseabilities(state) and state.engine.isserver)
        {
            ownerplayer.ultcharge.reset();
            ownerplayer.ultcharge.active = false;
            useultimate(state);
            state.engine.sendbuffer.write<uint8_t>(ULTIMATE_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }

        if (isflipped != (mouse_x < x))
        {
            // Spinjumping (compensate for later gravity)
            vspeed -= 540.0*frametime*3.0/4.0;
            isflipped = (mouse_x < x);
        }
    }

    // Gravity
    vspeed += 540.0*frametime;

    // apply friction
    hspeed *= std::pow(friction, frametime);

    // Passive ult charge
    ownerplayer.ultcharge.update(state, frametime*passiveultcharge());

    getweapon(state).beginstep(state, frametime);
}

void Character::midstep(Gamestate &state, double frametime)
{
    MovingEntity::midstep(state, frametime);

    // Collision with wallmask
    if (state.currentmap->collides(getcollisionrect(state)) and not pinanim.active())
    {
        // We collide, do collision handling
        double hs = hspeed*frametime, vs = vspeed*frametime;
        // Buffers for "undone" horizontal/vertical movement
        double xbuffer = 0.0, ybuffer = 0.0;
        int steps = std::ceil(std::fmax(std::abs(hs), std::abs(vs)));
        double xstep = hs/steps, ystep = vs/steps;
        // Pull us out of the wall
        for (int i=0; i<steps; ++i)
        {
            x -= xstep; y -= ystep;
            xbuffer += xstep; ybuffer += ystep;
            if (not state.currentmap->collides(getcollisionrect(state)))
            {
                break;
            }
        }
        // We're at the point where the character touched the wallmask for the first time
        // Now keep moving one unit in either direction until all possible movement is exhausted
        bool xblocked = false, yblocked = false;
        bool xfinished = false, yfinished = false;
        double oldxbuffer = xbuffer, oldybuffer = ybuffer;
        while (not xfinished or not yfinished)
        {
            oldxbuffer = xbuffer; oldybuffer = ybuffer;
            // Try first moving horizontally
            if (not xfinished)
            {
                if (not state.currentmap->collides(getcollisionrect(state).offset(xstep, 0)))
                {
                    x += xstep;
                    xbuffer -= xstep;
                    if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                    {
                        // We're done for this coordinate
                        xfinished = true;
                    }
                }
                else
                {
                    // There's the possibility we might have walked into a staircase
                    if (not state.currentmap->collides(getcollisionrect(state).offset(xstep, STAIRCASE_STEPSIZE)))
                    {
                        // Indeed we did. Move up
                        x += xstep;
                        y += STAIRCASE_STEPSIZE;
                        xbuffer -= xstep;
                        if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                        {
                            // We're done for this coordinate
                            xfinished = true;
                        }
                    }
                    else
                    {
                        // No more moving, we're blocked
                        xblocked = true;
                        xfinished = true;
                    }
                }
            }
            // Do the same vertically, but without stair code
            if (not yfinished)
            {
                if (not state.currentmap->collides(getcollisionrect(state).offset(0, ystep)))
                {
                    // We cam move through
                    y += ystep;
                    ybuffer -= ystep;
                    if (ybuffer == 0.0 or std::signbit(ybuffer) != std::signbit(oldybuffer))
                    {
                        // We're done for this coordinate
                        yfinished = true;
                    }
                }
                else
                {
                    // No more moving, we're blocked
                    yblocked = true;
                    yfinished = true;
                }
            }
        }
        // Set hspeed and vspeed to what they should be
        if (xblocked)
        {
            hspeed = 0;
            xblockedsmooth.reset();
        }
        if (yblocked)
        {
            vspeed = 0;
            yblockedsmooth.reset();
        }
        if (state.currentmap->collides(getcollisionrect(state)))
        {
            printf("\nError: Still in wallmask after collision handling.");
        }
    } // end collision with wallmask

    // Stick to stairs when going down
    if (std::fabs(vspeed*frametime) <= 3.0 and not std::signbit(vspeed*frametime) and not onground(state))
    {
        Rect r = getcollisionrect(state);
        int stepsize = (-STAIRCASE_STEPSIZE)*std::ceil(std::fabs(hspeed)*frametime/(-STAIRCASE_STEPSIZE));
        if (state.currentmap->collides(Rect(r.x, r.y+r.h+stepsize, r.w, 1)))
        {
            for (int i=0; i<stepsize; ++i)
            {
                ++y;
                if (onground(state))
                {
                    break;
                }
            }
        }
    }

    // Running animation
    if (onground(state))
    {
        if (isflipped)
        {
            runanim.update(state, -hspeed*frametime);
            crouchanim.update(state, -hspeed*frametime);
        }
        else
        {
            runanim.update(state, hspeed*frametime);
            crouchanim.update(state, hspeed*frametime);
        }
    }
    stunanim.update(state, frametime);
    earthshatteredfallanim.update(state, frametime);
    earthshatteredanim.update(state, frametime);
    earthshatteredgetupanim.update(state, frametime);
    pinanim.update(state, frametime);
    ongroundsmooth.update(state, frametime);
    xblockedsmooth.update(state, frametime);
    yblockedsmooth.update(state, frametime);
    jumpcooldown.update(state, frametime);
    if (hspeed == 0.0)
    {
        bool run=runanim.active(), crouch=crouchanim.active();
        runanim.reset();
        runanim.active(run);
        crouchanim.reset();
        crouchanim.active(crouch);
    }

    getweapon(state).midstep(state, frametime);
}

void Character::endstep(Gamestate &state, double frametime)
{
    getweapon(state).endstep(state, frametime);
}

void Character::render(Renderer &renderer, Gamestate &state)
{
    // --------------- HEALTHBAR ---------------
    al_set_target_bitmap(renderer.surfaceground);
    std::string mainsprite = currentsprite(state, false);

    ALLEGRO_COLOR healthcolors[] = { ColorPalette::premul(Color::HP, 255),
                                      ColorPalette::premul(Color::ARMOR, 255),
                                      ColorPalette::premul(Color::TORBARMOR, 255),
                                      ColorPalette::premul(Color::SHIELD, 255),
                                      ColorPalette::premul(Color::SHIELD, 255),
                                      ColorPalette::premul(Color::LUCIOSHIELD, 255),
                                      ColorPalette::premul(Color::HP, 51)};

    double healthamounts[] = { hp.normal,
                               hp.armor,
                               hp.torbarmor,
                               hp.shields,
                               hp.symshields,
                               hp.lucioshields,
                               std::max(hp.max()-hp.total(), 0.0)};
    int healthamounts_length = sizeof(healthamounts) / sizeof(healthamounts[0]);

    double center_x = renderer.zoom * (x - renderer.cam_x);
    double health_top_y = renderer.zoom * (y - renderer.spriteloader.get_spriteoffset_y(mainsprite) - renderer.cam_y - 15);
    double totalwidth = renderer.zoom * 60;
    double between_rect_spacing = 2;
    double slant = 0.3;
    double health_height = renderer.zoom * 6;
    double hp_per_rect = 25;

    int nrects = static_cast<int>(std::ceil((hp.max()+hp.torbarmor+hp.lucioshields)/hp_per_rect));
    double rect_width = (totalwidth - between_rect_spacing*(nrects-1)) / nrects;
    if (rect_width < 1)
    {
        // If we have a lot of health on a small window, remove spacing and make a continuous line
        between_rect_spacing = 0;
        rect_width = totalwidth / nrects;
    }
    float r[8]; // Array used to pass the polygon data for the actual drawing

    for (int rect = 0; rect < nrects; ++rect)
    {
        double rect_x = center_x + (rect - nrects/2.0) * (rect_width)
                        + std::floor(rect - nrects/2.0 + 0.5) * (between_rect_spacing);

        double recthp = hp_per_rect;
        for (int i=0; i<healthamounts_length; ++i)
        {
            double &healthamount = healthamounts[i];
            if (healthamount >= recthp)
            {
                double rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
                // Topleft
                r[0] = rect_x + rect_width * rectpercentage + health_height*slant;
                r[1] = health_top_y;

                // Bottomleft
                r[2] = rect_x + rect_width * rectpercentage;
                r[3] = health_top_y + health_height;

                // Bottomright
                r[4] = rect_x + rect_width;
                r[5] = health_top_y + health_height;

                // Topright
                r[6] = rect_x + rect_width + health_height*slant;
                r[7] = health_top_y;

                al_draw_filled_polygon(r, 4, healthcolors[i]);

                healthamount -= recthp;
                // We're done here, no need to keep this iteration
                break;
            }
            else
            {
                // We need to draw several health types into a single rect
                // This is where it gets fun
                double start_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
                recthp -= healthamount;
                healthamount = 0;
                double end_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;

                // Topleft
                r[0] = rect_x + rect_width * start_rectpercentage + health_height*slant;
                r[1] = health_top_y;

                // Bottomleft
                r[2] = rect_x + rect_width * start_rectpercentage;
                r[3] = health_top_y + health_height;

                // Bottomright
                r[4] = rect_x + rect_width * end_rectpercentage;
                r[5] = health_top_y + health_height;

                // Topright
                r[6] = rect_x + rect_width * end_rectpercentage + health_height*slant;
                r[7] = health_top_y;

                al_draw_filled_polygon(r, 4, healthcolors[i]);
            }
        }
    }
    // --------------- /HEALTHBAR ---------------


    // Deadeye circle
    Player &player = state.get<Player>(renderer.myself);
    if (player.heroclass == MCCREE and player.team != team)
    {
        if (state.exists(player.character))
        {
            Mccree &c = state.get<Mccree>(player.character);
            if (c.ulting.active)
            {
                double charge = 0;
                Peacemaker &w = state.get<Peacemaker>(c.weapon);
                if (w.deadeyetargets.count(owner.id) > 0)
                {
                    charge = w.deadeyetargets.at(owner.id);
                }

                al_set_target_bitmap(renderer.foreground);
                std::string mainsprite = "ui/ingame/heroes/mccree/lockon";
                ALLEGRO_BITMAP *skull = renderer.spriteloader.requestsprite(mainsprite);
                double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
                double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
                double rel_x = (x - renderer.cam_x)*renderer.zoom;
                double rel_y = (y - renderer.cam_y)*renderer.zoom;
                double factor = (hp.total()-charge) / initializehealth().total();
                if (factor < 0)
                {
                    al_draw_bitmap(skull, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
                    factor = 0;
                }
                al_draw_circle(rel_x, rel_y, 8+32*factor, al_map_rgb(253, 58, 58), 1);
            }
        }
    }

    // Name
    auto &font = renderer.font8;
    int text_top_y = health_top_y - al_get_font_line_height(font) - 10 * renderer.zoom;
    al_draw_text(font, al_map_rgb(255, 255, 255), center_x, text_top_y, ALLEGRO_ALIGN_CENTER,
                 state.get<Player&>(owner).name.c_str());
}

bool Character::onground(Gamestate &state)
{
    Rect r = getcollisionrect(state);
    if (state.currentmap->collides(Rect(r.x, r.y+r.h, r.w, 1)))
    {
        ongroundsmooth.reset();
        return true;
    }
    else
    {
        return false;
    }
}

bool Character::cangetinput(Gamestate &state)
{
    return not stunanim.active() and not pinanim.active() and not earthshatteredfallanim.active()
           and not earthshatteredanim.active() and not earthshatteredgetupanim.active();
}

bool Character::weaponvisible(Gamestate &state)
{
    return not earthshatteredfallanim.active() and not earthshatteredanim.active()
           and not earthshatteredgetupanim.active() and not pinanim.active();
}

double Character::maxdamageabledist(Gamestate &state, double *centerx, double *centery)
{
    *centerx = x;
    *centery = y;
    Rect bbox = state.engine.maskloader.get_rect(currentsprite(state, true));
    return std::hypot(bbox.w, bbox.h);
}

void Character::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Character &p = static_cast<Character&>(prev_entity);
    Character &n = static_cast<Character&>(next_entity);

    if (alpha < 0.5)
    {
        heldkeys = p.heldkeys;
        crouchanim.active(p.crouchanim.active());
    }
    else
    {
        heldkeys = n.heldkeys;
        crouchanim.active(n.crouchanim.active());
    }
    mouse_x = p.mouse_x + alpha*(n.mouse_x - p.mouse_x);
    mouse_y = p.mouse_y + alpha*(n.mouse_y - p.mouse_y);
    runanim.interpolate(p.runanim, n.runanim, alpha);
    crouchanim.interpolate(p.crouchanim, n.crouchanim, alpha);
    stunanim.interpolate(p.stunanim, n.stunanim, alpha);
    pinanim.interpolate(p.pinanim, n.pinanim, alpha);
    earthshatteredfallanim.interpolate(p.earthshatteredfallanim, n.earthshatteredfallanim, alpha);
    earthshatteredanim.interpolate(p.earthshatteredanim, n.earthshatteredanim, alpha);
    earthshatteredgetupanim.interpolate(p.earthshatteredgetupanim, n.earthshatteredgetupanim, alpha);
    ongroundsmooth.interpolate(p.ongroundsmooth, n.ongroundsmooth, alpha);
    xblockedsmooth.interpolate(p.xblockedsmooth, n.xblockedsmooth, alpha);
    yblockedsmooth.interpolate(p.yblockedsmooth, n.yblockedsmooth, alpha);
    jumpcooldown.interpolate(p.jumpcooldown, n.jumpcooldown, alpha);
    hp.normal = p.hp.normal + alpha*(n.hp.normal - p.hp.normal);
    hp.armor = p.hp.armor + alpha*(n.hp.armor - p.hp.armor);
    hp.shields = p.hp.shields + alpha*(n.hp.shields - p.hp.shields);
}

void Character::serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate)
{
    MovingEntity::serialize(state, buffer, fullupdate);

    buffer.write<uint16_t>(hp.normal);
    buffer.write<uint16_t>(hp.armor);
    buffer.write<uint16_t>(hp.shields);

    ReducedInputContainer input = heldkeys.reduce();
    input.serialize(buffer);

    buffer.write<int16_t>(mouse_x);
    buffer.write<int16_t>(mouse_y);

    getweapon(state).serialize(state, buffer, fullupdate);
}

void Character::deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate)
{
    MovingEntity::deserialize(state, buffer, fullupdate);

    hp.normal = buffer.read<uint16_t>();
    hp.armor = buffer.read<uint16_t>();
    hp.shields = buffer.read<uint16_t>();

    ReducedInputContainer input;
    input.deserialize(buffer);
    heldkeys.update(input);

    mouse_x = buffer.read<int16_t>();
    mouse_y = buffer.read<int16_t>();
    getweapon(state).setaim(mouse_x, mouse_y);

    getweapon(state).deserialize(state, buffer, fullupdate);
}

bool Character::collides(Gamestate &state, double testx, double testy)
{
    Rect self = state.engine.maskloader.get_rect(currentsprite(state, true)).offset(x, y);

    if (testx > self.x and testx < self.x+self.w and testy > self.y and testy < self.y+self.h)
    {
        // We're close enough that an actual collision might happen
        // Check the sprite
        ALLEGRO_BITMAP *selfsprite = state.engine.maskloader.requestsprite(currentsprite(state, true));
        return al_get_pixel(selfsprite, testx-self.x, testy-self.y).a != 0;
    }
    else
    {
        return false;
    }
}

double Character::damage(Gamestate &state, double amount)
{
    double effective_damage = hp.damage(amount);
    if (hp.total() <= 0)
    {
        die(state);
    }
    return effective_damage;
}

void Character::die(Gamestate &state)
{
    if (state.engine.isserver)
    {
        destroy(state);

        state.engine.sendbuffer.write<uint8_t>(PLAYER_DIED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));

        state.get<Player>(owner).spawntimer.reset();

        // If we're in the spawnroom, respawn immediately
        if (state.currentmap->spawnroom(state, team).isinside(x, y))
        {
            state.get<Player>(owner).spawntimer.timer = state.get<Player>(owner).spawntimer.duration;
        }
    }
}

void Character::stun(Gamestate &state)
{
    stunanim.reset();
    interrupt(state);
}

void Character::destroy(Gamestate &state)
{
    state.get<Player>(owner).character = 0;
    state.get<Player>(owner).ultcharge.active = true;
    getweapon(state).destroy(state);
    Corpse &c = state.get<Corpse>(state.make_entity<Corpse>(state, herofolder(), isflipped));
    c.x = x;
    c.y = y;

    MovingEntity::destroy(state);
}

Weapon& Character::getweapon(Gamestate &state)
{
    return state.get<Weapon>(weapon);
}
