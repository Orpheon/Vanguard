#pragma once

#include "gamestate.h"

class Hud
{
    public:
        virtual void render(Renderer &renderer, Gamestate &state, Player &myself) = 0;
};