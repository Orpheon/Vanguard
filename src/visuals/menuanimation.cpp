#include "visuals/menuanimation.h"

#include "configloader.h"
#include "global.h"


void MenuAnimation::init(std::string path_)
{
    path = path_;

    ConfigLoader cfgloader;

    nlohmann::json data = cfgloader.open("gamedata.json");
    try
    {
        duration = data[path+" duration"];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load %s animation duration", path.c_str());
    }

    nlohmann::json data2 = cfgloader.open("sprites/spritedata.json");
    try
    {
        nframes = data2[path+" number of frames"];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load %s animation number of frames", path.c_str());
    }

    timer = 0;
    inited = true;
}

std::string MenuAnimation::getframepath()
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Animation getframe() called before initialized!");

    }

    int f = static_cast<int>(std::floor(nframes*timer/duration))+1;
    f = std::min(std::max(f, 1), nframes);
    return path + std::to_string(f);
}

void MenuAnimation::update(double dt)
{
    if (timer < duration)
    {
        timer += dt;
    }
}

void MenuLoopAnimation::update(double dt)
{
    while (timer > duration)
    {
        timer -= duration;
    }
    timer += dt;
}