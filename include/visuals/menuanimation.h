#pragma once

#include <string>

// This is as a replacement for Animation and LoopAnimation as it exists in the rest of the game
// without dependence on gamestate and other stuff

class MenuAnimation
{
    public:
        virtual void init(std::string path_);
        virtual void update(double dt);
        virtual std::string getframepath();
        virtual bool active() { return timer < duration; }

    protected:
        double timer;
        double duration;
        std::string path;
        int nframes;
        bool inited = false;
};

class MenuLoopAnimation : public MenuAnimation
{
    public:
        void update(double dt) override;
        bool active() override { return true; }
};