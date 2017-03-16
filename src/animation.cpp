#include "animation.h"
#include "json.hpp"
#include "configloader.h"

#include <fstream>
#include <cmath>

void Animation::init(std::string path_, std::function<void(Gamestate *state)> eventfunc_)
{
    path = path_;

    ConfigLoader cfgloader;

    nlohmann::json data = cfgloader.requestconfig("gamedata.json");
    double duration = 0;
    try
    {
        duration = data[path+" duration"];
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "Error: Could not load %s animation duration!", path.c_str());
        throw -1;
    }

    nlohmann::json data2 = cfgloader.requestconfig("sprites/spritedata.json");
    try
    {
        nframes = data2[path+" number of frames"];
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "Error: Could not load %s animation number of frames!", path.c_str());
        throw -1;
    }

    timer.init(duration, eventfunc_);
    inited = true;
}

void Animation::init(std::string path_)
{
    Animation::init(path_, nullptr);
}

int Animation::getframe()
{
    if (not inited)
    {
        std::cout << "Fatal Error: Animation used before initialized!";
        throw -1;
    }

    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 1), nframes);
    return f;
}

std::string Animation::getframepath()
{
    if (not inited)
    {
        std::cout << "Fatal Error: Animation used before initialized!";
        throw -1;
    }

    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 1), nframes);
    return path+std::to_string(getframe());
}

void Animation::update(Gamestate *state, double dt)
{
    if (not inited)
    {
        std::cout << "Fatal Error: Animation used before initialized!";
        throw -1;
    }

    timer.update(state, dt);
}

void Animation::interpolate(Animation *prev_anim, Animation *next_anim, double alpha)
{
    if (not inited)
    {
        std::cout << "Fatal Error: Animation used before initialized!";
        throw -1;
    }

    timer.interpolate(&(prev_anim->timer), &(next_anim->timer), alpha);
}

void Animation::Animation::reset()
{
    if (not inited)
    {
        std::cout << "Fatal Error: Animation used before initialized!";
        throw -1;
    }

    timer.reset();
}


// ######### LOOP ANIMATION #########

void LoopAnimation::update(Gamestate *state, double dt)
{
    if (not inited)
    {
        std::cout << "Fatal Error: LoopAnimation used before initialized!";
        throw -1;
    }

    timer.update(state, dt);
    if (not timer.active and timer.timer >= timer.duration)
    {
        timer.timer -= timer.duration;
        timer.active = true;
    }
    if (timer.timer < 0)
    {
        timer.timer += timer.duration;
    }
}

void LoopAnimation::interpolate(Animation *prev_anim, Animation *next_anim, double alpha)
{
    if (not inited)
    {
        std::cout << "Fatal Error: LoopAnimation used before initialized!";
        throw -1;
    }

    if (std::abs(prev_anim->timer.timer - next_anim->timer.timer) > 0.5*timer.duration)
    {
        // We've probably looped over
        if (prev_anim->timer.timer < next_anim->timer.timer)
        {
            prev_anim->timer.timer += timer.duration;
            timer.interpolate(&(prev_anim->timer), &(next_anim->timer), alpha);
            prev_anim->timer.timer -= timer.duration;
        }
        else
        {
            next_anim->timer.timer += timer.duration;
            timer.interpolate(&(prev_anim->timer), &(next_anim->timer), alpha);
            next_anim->timer.timer -= timer.duration;
        }
    }
    else
    {
        timer.interpolate(&(prev_anim->timer), &(next_anim->timer), alpha);
    }
}
