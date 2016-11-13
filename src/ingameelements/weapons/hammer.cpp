#include <cmath>

#include "ingameelements/weapons/hammer.h"
#include "renderer.h"
#include "ingameelements/heroes/reinhardt.h"

#include "engine.h"

Hammer::Hammer(uint64_t id_, Gamestate *state, EntityPtr owner_) : Weapon(id_, state, owner_, constructparameters(state))
{

}

Hammer::~Hammer()
{
    //dtor
}

void Hammer::renderbehind(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite;
    Reinhardt *c = state->get<Reinhardt>(state->get<Player>(owner)->character);
    mainsprite = c->getcharacterfolder()+"arm/armback";
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (c->weaponvisible(state))
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, -getattachpoint_x()-spriteoffset_x, getattachpoint_y()+spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, 1, -1, 0, 0);
        }
        else
        {
            al_draw_bitmap(sprite, x - (getattachpoint_x()+spriteoffset_x) - renderer->cam_x, y - (getattachpoint_y()+spriteoffset_y) - renderer->cam_y, 0);
        }
    }
}

void Hammer::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite;
    Reinhardt *c = state->get<Reinhardt>(state->get<Player>(owner)->character);
    mainsprite = c->getcharacterfolder()+"arm/armfront";
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (c->weaponvisible(state))
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, -getattachpoint_x()-spriteoffset_x, getattachpoint_y()+spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, 1, -1, 0, 0);
        }
        else
        {
            al_draw_bitmap(sprite, x - (getattachpoint_x()+spriteoffset_x) - renderer->cam_x, y - (getattachpoint_y()+spriteoffset_y) - renderer->cam_y, 0);
        }
    }
}

void Hammer::midstep(Gamestate *state, double frametime)
{
    Weapon::midstep(state, frametime);
}

void Hammer::reload(Gamestate *state)
{

}

void Hammer::wantfireprimary(Gamestate *state)
{
    if (state->engine->isserver and clip > 0 and not firinganim.active())
    {
        fireprimary(state);
        state->sendbuffer->write<uint8_t>(PRIMARY_FIRED);
        state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
    }
}

void Hammer::fireprimary(Gamestate *state)
{


    firinganim.reset();
    firinganim.active(true);
}

void Hammer::wantfiresecondary(Gamestate *state)
{
    firesecondary(state);
    state->sendbuffer->write<uint8_t>(SECONDARY_FIRED);
    state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
}

void Hammer::firesecondary(Gamestate *state)
{

}

WeaponChildParameters Hammer::constructparameters(Gamestate *state)
{
    WeaponChildParameters p;
    p.clipsize = 1;
    p.characterfolder = "heroes/reinhardt/";
    p.reloadfunction = std::bind(&Hammer::restoreclip, this, state);
    return p;
}
