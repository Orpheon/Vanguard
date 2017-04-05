#include <cmath>

#include "ingameelements/weapons/hammer.h"
#include "renderer.h"
#include "ingameelements/heroes/reinhardt.h"

#include "engine.h"

void Hammer::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Weapon::init(id_, state, owner_);

    barrier.init(0.1);
    barrier.active = false;
    barrierrecharge.init(2);
    barrierrecharge.active = false;
    barrierbreak.init(5);
    barrierbreak.active = false;
}

void Hammer::renderbehind(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    Reinhardt *c = state.get<Reinhardt>(state.get<Player>(owner)->character);
    if (barrier.active)
    {
        mainsprite = "heroes/reinhardt/shield/back";

    }
    else
    {
        mainsprite = "heroes/reinhardt/arm/back";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getattachpoint_x()*renderer.zoom;
    double attachpt_y = getattachpoint_y()*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c->weaponvisible(state))
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, (aimdirection+3.1415)*barrier.active, 0);
            if (state.get<Player>(renderer.myself)->team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y,
                                                    -1, 1, (aimdirection+3.1415)*barrier.active, 0);
            }
        }
        else
        {
            al_draw_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, aimdirection*barrier.active, 0);
            if (state.get<Player>(renderer.myself)->team != team)
            {
                // Draw enemy outline
                al_draw_tinted_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, aimdirection*barrier.active, 0);
            }
        }
    }
}

void Hammer::render(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    Reinhardt *c = state.get<Reinhardt>(state.get<Player>(owner)->character);
    if (barrier.active)
    {
        mainsprite = "heroes/reinhardt/shield/front";

    }
    else
    {
        mainsprite = "heroes/reinhardt/arm/front";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getattachpoint_x()*renderer.zoom;
    double attachpt_y = getattachpoint_y()*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c->weaponvisible(state))
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, -attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            if (state.get<Player>(renderer.myself)->team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            }
        }
        else
        {
            al_draw_bitmap(sprite, rel_x - (attachpt_x+spriteoffset_x), rel_y - (attachpt_y+spriteoffset_y), 0);
            if (state.get<Player>(renderer.myself)->team != team)
            {
                // Draw enemy outline
                al_draw_tinted_bitmap(outline, outlinecolor, rel_x - (attachpt_x+spriteoffset_x), rel_y - (attachpt_y+spriteoffset_y), 0);
            }
        }
    }

    // Barrier drawing code, move into its own entity once it exists

//    if (barrier.active)
//    {
//        if (barrierhealth/MAX_BARRIER_HEALTH >= 0.6)
//        {
//            mainsprite = "heroes/reinhardt/shield/0%";
//        }
//        else if (barrierhealth/MAX_BARRIER_HEALTH >= 0.2)
//        {
//            mainsprite = "heroes/reinhardt/shield/40%";
//        }
//        else
//        {
//            mainsprite = "heroes/reinhardt/shield/80%";
//        }
//        sprite = renderer.spriteloader.requestsprite(mainsprite);
//        spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
//        spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
//        rel_x = (x - renderer.cam_x)*renderer.zoom;
//        rel_y = (y - renderer.cam_y)*renderer.zoom;
//        attachpt_x = getattachpoint_x()*renderer.zoom;
//        attachpt_y = getattachpoint_y()*renderer.zoom;
//
//        if (c->weaponvisible(state))
//        {
//            if (c->isflipped)
//            {
//                al_draw_scaled_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, (aimdirection+3.1415)*barrier.active, 0);
//            }
//            else
//            {
//                al_draw_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, aimdirection*barrier.active, 0);
//            }
//        }
//    }
}

void Hammer::beginstep(Gamestate &state, double frametime)
{
    Weapon::beginstep(state, frametime);
}

void Hammer::midstep(Gamestate &state, double frametime)
{
    Weapon::midstep(state, frametime);
    barrierrecharge.update(state, frametime);
    barrierbreak.update(state, frametime);
    barrier.update(state, frametime);
}

void Hammer::wantfireprimary(Gamestate &state)
{
    if (state.engine->isserver and not firinganim.active())
    {
        fireprimary(state);
        state.engine->sendbuffer->write<uint8_t>(PRIMARY_FIRED);
        state.engine->sendbuffer->write<uint8_t>(state.findplayerid(owner));
    }
}

void Hammer::fireprimary(Gamestate &state)
{
    firinganim.reset();
    firinganim.active(true);
}

void Hammer::wantfiresecondary(Gamestate &state)
{
    firesecondary(state);
    state.engine->sendbuffer->write<uint8_t>(SECONDARY_FIRED);
    state.engine->sendbuffer->write<uint8_t>(state.findplayerid(owner));
}

void Hammer::firesecondary(Gamestate &state)
{
    barrier.reset();
    barrierrecharge.reset();
    barrierrecharge.active = false;
}
