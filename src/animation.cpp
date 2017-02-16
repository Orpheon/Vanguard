#include "animation.h"

#include "json.hpp"
#include <fstream>
#include <cmath>

void Animation::init(std::string path_, std::function<void(Gamestate *state)> eventfunc_)
{
    path = path_;

    std::ifstream datafile("gamedata.json");
    nlohmann::json data;
    data << datafile;
    datafile.close();
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

    std::ifstream datafile2("sprites/spritedata.json");
    nlohmann::json data2;
    data2 << datafile2;
    datafile2.close();
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
}

void Animation::init(std::string path_)
{
    Animation::init(path_, nullptr);
}

int Animation::getframe()
{
    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 1), nframes);
    return f;
}

std::string Animation::getframepath()
{
    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 1), nframes);
    return path+std::to_string(getframe());
}

void Animation::update(Gamestate *state, double dt)
{
    timer.update(state, dt);
}

void Animation::interpolate(Animation *prev_anim, Animation *next_anim, double alpha)
{
    timer.interpolate(&(prev_anim->timer), &(next_anim->timer), alpha);
}

void Animation::Animation::reset()
{
    timer.reset();
}


// ######### LOOP ANIMATION #########

void LoopAnimation::update(Gamestate *state, double dt)
{
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
