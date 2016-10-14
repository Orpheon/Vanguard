#include "animation.h"

#include "json.hpp"
#include <fstream>
#include <cmath>

Animation::Animation(std::string path_) : timer(0, 0), path(path_)
{
    std::ifstream datafile("spriteoffsets.json");
    nlohmann::json data;
    data << datafile;
    datafile.close();
    nframes = data[path+" number of frames"];

    timer.duration = data[path+" duration"];
}

Animation::Animation(std::string path_, std::function<void(Gamestate *state)> eventfunc_) : timer(eventfunc_, 0), path(path_)
{
    std::ifstream datafile("spriteoffsets.json");
    nlohmann::json data;
    data << datafile;
    datafile.close();
    nframes = data[path+" number of frames"];

    timer.duration = data[path+" duration"];
}

Animation::~Animation()
{
    //dtor
}

std::string Animation::getframe()
{
    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 0), nframes);
    return path+std::to_string(f)+".png";
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
