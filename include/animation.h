#ifndef ANIMATION_H
#define ANIMATION_H

#include <allegro5/allegro.h>
#include <string>

class Animation
{
    public:
        Animation();
        Animation(std::string path_);
        virtual ~Animation();
        virtual std::string get_frame();
        bool update(double modification);
        double gettimer() {return timer;}
        void interpolate(double t1, double t2, double alpha);
    protected:
        double duration;
        double timer;
        int nframes;
        std::string path;
};


class LoopAnimation : public Animation
{
    public:
        LoopAnimation();
        LoopAnimation(std::string path_);
        virtual ~LoopAnimation();
        void update(double modification);
        void reset();
};

#endif // ANIMATION_H
