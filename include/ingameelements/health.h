#pragma once

#include "timer.h"

class Gamestate;

class Health
{
    public:
        Health() {}
        Health(double n, double a, double s);
        double damage(double amount);
        double heal(double amount);
        void fill();
        double total();
        double max();
        void update(Gamestate &state, double frametime);
        void interpolate(Health &prev_health, Health &next_health, double alpha);

        double normal;
        double armor;
        double shields;
        double torbarmor;
        double symshields;
        double lucioshields;
        double max_normal;
        double max_armor;
        double max_shields;
        double max_symshields;

    private:
        Timer damaged;

        static constexpr double SHIELD_RECHARGE_RATE = 30;
};