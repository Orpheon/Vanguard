#ifndef PEACEMAKER_H
#define PEACEMAKER_H

#include "weapon.h"

class Peacemaker : public Weapon
{
    public:
        Peacemaker(Gamestate *state, EntityPtr owner_);
        virtual ~Peacemaker();

        void render(Renderer *renderer, Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask) {return "";}
    protected:
    private:
        double xoffset = 0;
        double yoffset = 0;
};

#endif // PEACEMAKER_H
