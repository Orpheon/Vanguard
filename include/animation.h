#pragma once

#include <allegro5/allegro.h>
#include <string>
#include "timer.h"

class Animation
{
    public:
        virtual void init(std::string path_);
        virtual void init(std::string path_, std::function<void(Gamestate *state)> eventfunc_);
        virtual ~Animation() = default;
        virtual int getframe();
        virtual std::string getframepath();
        virtual void update(Gamestate *state, double dt);
        virtual double getpercent() {return timer.getpercent();}
        virtual void interpolate(Animation *prev_anim, Animation *next_anim, double alpha);
        virtual bool active() {return timer.active;}
        virtual void active(bool active_) {timer.active = active_;}
        virtual void reset();
        Timer timer;
        int nframes;
        std::string path;
        bool inited = false;
};


class LoopAnimation : public Animation
{
    public:
        virtual ~LoopAnimation() override = default;
        void update(Gamestate *state, double dt) override;
        void interpolate(Animation *prev_anim, Animation *next_anim, double alpha) override;
};

