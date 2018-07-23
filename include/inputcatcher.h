#pragma once

#include "datastructures.h"
#include "renderer.h"
#include "engine.h"
#include "json.hpp"

class InputCatcher
{
    public:
        InputCatcher();
        ~InputCatcher();
        void run(sf::RenderWindow &window, Gamestate &state, Networker &networker, Renderer &renderer, EntityPtr myself);
    private:
        nlohmann::json config;
};

