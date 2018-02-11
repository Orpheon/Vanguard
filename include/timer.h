#pragma once

#include "entity.h"
#include <functional>

// Circular reference
class Gamestate;

class Timer
{
    public:
        virtual ~Timer() = default;
        virtual void init(double duration_, std::function<void(Gamestate &state)> eventfunc_, bool active_);
        virtual void init(double duration_, bool active_);
        void update(Gamestate &state, double dt);
        double getpercent();
        void interpolate(Timer &prev_timer, Timer &next_timer, double alpha);
        void reset();
        void reset_after_eventfunc();
        double timer = 0;
        double duration = 0;
        bool active = false;
        bool inited = false;
        std::function<void(Gamestate &state)> eventfunc;
    private:
        bool reset_after_eventfunc_flag = false;
};

