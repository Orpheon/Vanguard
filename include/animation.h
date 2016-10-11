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
        virtual std::string getframe();
        void update(Gamestate *state, double dt);
        double getpercent() {return timer.getpercent();}
        void interpolate(Animation *prev_anim, Animation *next_anim, double alpha);
        bool active() {return timer.active;}
        void active(bool active_) {timer.active = active_;}
        void reset();
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
};

#endif // ANIMATION_H
