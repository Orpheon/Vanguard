#pragma once

#include "visuals/hud.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/heroes/reinhardt.h"
#include "ingameelements/heroes/lucio.h"

class DefaultHud : public Hud
{
    public:
        void render(Renderer &renderer, Gamestate &state, Player &myself) override;
        void renderkillfeed(Renderer &renderer, Gamestate &state, Player &myself);
        void renderhealthbar(Renderer &renderer, Gamestate &state, Character &character);
        void mccreehud(Renderer &renderer, Gamestate &state, Mccree &myself);
        void reinhardthud(Renderer &renderer, Gamestate &state, Reinhardt &myself);
        void luciohud(Renderer &renderer, Gamestate &state, Lucio &myself);
        double renderability(Renderer &renderer, std::string spritename, double x, double y, Timer cooldown,
                             bool active);

        double HUD_Y_BASELINE = 0;
        double KILLFEED_DURATION = 5;
};