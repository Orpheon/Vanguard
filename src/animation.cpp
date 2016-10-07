#include "animation.h"

#include "json.hpp"
#include <fstream>
#include <cmath>

Animation::Animation()
{
    printf("\nAnimation constructor called, this should really never happen.");
}

Animation::Animation(std::string path_) : path(path_)
{
    timer = 0;
    std::ifstream datafile("spriteoffsets.json");
    nlohmann::json data;
    data << datafile;
    datafile.close();
    nframes = data[path+" number of frames"];
    duration = data[path+" duration"];
}

Animation::~Animation()
{
    //dtor
}

std::string Animation::get_frame()
{
    return path+std::to_string(static_cast<int>(std::floor(nframes*timer/duration))+1)+".png";
}

bool Animation::update(double modification)
{
    timer += modification;
    if (timer >= duration)
    {
        return true;
    }
    return false;
}

void Animation::interpolate(double t1, double t2, double alpha)
{
    timer = t1 + alpha*(t2-t1);
}




LoopAnimation::LoopAnimation(std::string path_) : Animation::Animation(path_)
{
    ;
}

LoopAnimation::~LoopAnimation()
{
    // dtor
}

void LoopAnimation::update(double modification)
{
    timer += modification;
    // Round timer
    timer = std::round(timer*1000.0)/1000.0;
    while (timer >= duration)
    {
        // Yes, we could use fmod, but apparently that can give some ugly rounding errors so this is safer
        timer -= duration;
    }
    while (timer < -0.0)
    {
        timer += duration;
    }
}

void LoopAnimation::reset()
{
    timer = 0;
}
