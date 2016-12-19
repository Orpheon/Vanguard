#include "animation.h"

#include "json.hpp"
#include <fstream>
#include <cmath>

Animation::Animation(std::string path_) : timer(0, 0), path(path_)
{
    loaddata();
}

Animation::Animation(std::string path_, std::function<void(Gamestate *state)> eventfunc_) : timer(eventfunc_, 0), path(path_)
{
    loaddata();
}

Animation::~Animation()
{
    //dtor
}

void Animation::loaddata()
{
    std::ifstream datafile("gamedata.json");
    nlohmann::json data;
    data << datafile;
    datafile.close();
    try
    {
        timer.duration = data[path+" duration"];
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



LoopAnimation::LoopAnimation(std::string path_) : Animation::Animation(path_)
{
    ;
}

LoopAnimation::LoopAnimation(std::string path_, std::function<void(Gamestate *state)> eventfunc_) : Animation::Animation(path_, eventfunc_)
{
    ;
}

LoopAnimation::~LoopAnimation()
{
    // dtor
}

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
