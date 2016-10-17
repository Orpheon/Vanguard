#ifndef TIMER_H
#define TIMER_H

#include "entity.h"
#include <functional>

// Circular reference
class Gamestate;

class Timer
{
    public:
        Timer(std::function<void(Gamestate *state)> eventfunc_, double duration_);
        Timer(double duration_);
        virtual ~Timer();
        void update(Gamestate *state, double dt);
        double getpercent();
        void interpolate(Timer *prev_timer, Timer *next_timer, double alpha);
        void reset();
        double timer;
        double duration;
        bool active;
        std::function<void(Gamestate *state)> eventfunc;
};

#endif // TIMER_H
