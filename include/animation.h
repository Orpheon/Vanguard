#ifndef ANIMATION_H
#define ANIMATION_H

#include <allegro5/allegro.h>
#include <string>
#include "timer.h"

class Animation
{
    public:
        Animation(std::string path_);
        Animation(std::string path_, std::function<void(Gamestate *state)> eventfunc_);
        virtual ~Animation();
        virtual void loaddata();
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
};


class LoopAnimation : public Animation
{
    public:
        LoopAnimation(std::string path_);
        LoopAnimation(std::string path_, std::function<void(Gamestate *state)> eventfunc_);
        virtual ~LoopAnimation();
        void update(Gamestate *state, double dt) override;
        void interpolate(Animation *prev_anim, Animation *next_anim, double alpha) override;
};

#endif // ANIMATION_H
