#ifndef MCCREE_H
#define MCCREE_H

#include <character.h>


class Mccree : public Character
{
    public:
        Mccree(Gamestate *state);
        virtual ~Mccree();
        void render();
        Mccree* clone(Gamestate *state);
    protected:
    private:
};

#endif // MCCREE_H
