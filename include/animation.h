#ifndef ANIMATION_H
#define ANIMATION_H

#include <allegro5/allegro.h>
#include <string>
#include "timer.h"

class Animation
{
    public:
        Animation(std::string path_);
        Animation(std::string path_, void (*eventfunc_)(Gamestate *state));
        virtual ~Animation();
        virtual std::string get_frame();
        void update(Gamestate *state, double dt);
        double getpercent() {return timer.getpercent();}
        void interpolate(Animation *prev_anim, Animation *next_anim, double alpha);
        Timer timer;
        int nframes;
        std::string path;
};


class LoopAnimation : public Animation
{
    public:
        LoopAnimation(std::string path_);
        LoopAnimation(std::string path_, void (*eventfunc_)(Gamestate *state));
        virtual ~LoopAnimation();
        void update(Gamestate *state, double dt);
        void reset();
};

#endif // ANIMATION_H
