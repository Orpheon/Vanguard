#ifndef TIMER_H
#define TIMER_H

// Circular reference
class Gamestate;

class Timer
{
    public:
        Timer(void (*eventfunc_)(Gamestate *state), double duration_);
        virtual ~Timer();
        void update(Gamestate *state, double dt);
        double getpercent();
        void interpolate(Timer *prev_timer, Timer *next_timer, double alpha);
        double timer;
        double duration;
        bool active;
        void (*eventfunc)(Gamestate *state);
};

#endif // TIMER_H
