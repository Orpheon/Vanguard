#pragma once

#include "visuals/hud.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/heroes/reinhardt.h"

class DefaultHud : public Hud
{
    public:
        void render(Renderer &renderer, Gamestate &state, Player &myself) override;
        void mccreehud(Renderer &renderer, Gamestate &state, Mccree &myself);
        void reinhardthud(Renderer &renderer, Gamestate &state, Reinhardt &myself);
        double renderability(Renderer &renderer, std::string spritename, double x, double y, Timer cooldown,
                             bool active);
};