
#include "ingameelements/character.h"
#include "ingameelements/weapons/reinhardtshield.h"
#include "gamestate.h"
#include "renderer.h"

void ReinhardtShield::init(uint64_t id_, Gamestate &state, Team team_, EntityPtr owner_)
{
    Shield::init(id_, state, SHIELD_MAX_HP, team_);
    owner = owner_;
    active = false;
    aimdirection = 0;
    rechargecooldown.init(2);
    rechargecooldown.active = false;
}

void ReinhardtShield::midstep(Gamestate &state, double frametime)
{
    Character &reinhardt = state.get<Player>(owner).getcharacter(state);
    active = reinhardt.heldkeys.SECONDARY_FIRE;
    rechargecooldown.update(state, frametime);
    if (active)
    {
        Weapon &hammer = reinhardt.getweapon(state);
        aimdirection = hammer.aimdirection;
        x = reinhardt.x + attachpoint_x() + DIST_TO_REINHARDT * std::cos(aimdirection);
        y = reinhardt.y + attachpoint_y() + DIST_TO_REINHARDT * std::sin(aimdirection);
    }
    else if (not rechargecooldown.active)
    {
        hp = std::max(SHIELD_MAX_HP, hp+frametime*SHIELD_RECHARGE);
    }
}

void ReinhardtShield::render(Renderer &renderer, Gamestate &state)
{
    if (active)
    {
        std::string mainsprite;
        ALLEGRO_BITMAP *sprite;
        double spriteoffset_x, spriteoffset_y;
        double rel_x, rel_y;
        double attachpt_x, attachpt_y;
        if (hp/SHIELD_MAX_HP >= 0.6)
        {
            mainsprite = "heroes/reinhardt/shield/0%";
        }
        else if (hp/SHIELD_MAX_HP >= 0.2)
        {
            mainsprite = "heroes/reinhardt/shield/40%";
        }
        else
        {
            mainsprite = "heroes/reinhardt/shield/80%";
        }
        sprite = renderer.spriteloader.requestsprite(mainsprite);
        spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
        spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
        rel_x = (x - renderer.cam_x)*renderer.zoom;
        rel_y = (y - renderer.cam_y)*renderer.zoom;
        attachpt_x = attachpoint_x()*renderer.zoom;
        attachpt_y = attachpoint_y()*renderer.zoom;

        Character &reinhardt = state.get<Player>(owner).getcharacter(state);

        if (reinhardt.weaponvisible(state))
        {
            if (reinhardt.isflipped)
            {
                al_draw_scaled_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, (aimdirection+3.1415)*active, 0);
            }
            else
            {
                al_draw_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, aimdirection*active, 0);
            }
        }
    }
}

void ReinhardtShield::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Shield::interpolate(prev_entity, next_entity, alpha);

    ReinhardtShield &prev_e = static_cast<ReinhardtShield&>(prev_entity);
    ReinhardtShield &next_e = static_cast<ReinhardtShield&>(next_entity);

    if (alpha < 0.5)
    {
        active = prev_e.active;
    }
    else
    {
        active = next_e.active;
    }
    rechargecooldown.interpolate(prev_e.rechargecooldown, next_e.rechargecooldown, alpha);
    aimdirection = prev_e.aimdirection + alpha*(next_e.aimdirection - prev_e.aimdirection);
}