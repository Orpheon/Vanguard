#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "renderer.h"
#include "datastructures.h"

#include <string>

class Mccree : public Character
{
    public:
        Mccree(Gamestate *state, PlayerPtr owner_);
        virtual ~Mccree() override;
        void render(Renderer *renderer, Gamestate *state) override;
        CharacterChildParameters constructCharacterChildParameters() override;
        Rect getcollisionrect(Gamestate *state) override;
        Rect getstandingcollisionrect(Gamestate *state) override;
        std::string getsprite(Gamestate *state, bool mask) override;
    protected:
    private:
};

#endif // MCCREE_H
