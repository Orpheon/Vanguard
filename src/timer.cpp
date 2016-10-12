#include <cmath>

#include "timer.h"

Timer::Timer(std::function<void(Gamestate *state)> eventfunc_, double duration_) : timer(0), duration(duration_), active(true), eventfunc(eventfunc_)
{
    ;
}

Timer::~Timer()
{
    //dtor
}

void Timer::update(Gamestate *state, double dt)
{
    if (active)
    {
        timer += dt;
        if (timer >= duration)
        {
            if (eventfunc != 0)
            {
                eventfunc(state);
            }
            active = false;
        }
    }
}

double Timer::getpercent()
{
    // Max and min are for rounding errors
    return std::fmax(std::fmin(timer/duration, 1.0), 0.0);
}

void Timer::interpolate(Timer *prev_timer, Timer *next_timer, double alpha)
{
    if (prev_timer->active and next_timer->active)
    {
        timer = prev_timer->timer + alpha*(next_timer->timer - prev_timer->timer);
    }
}
