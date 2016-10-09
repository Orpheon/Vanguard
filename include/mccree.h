#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "renderer.h"
#include "datastructures.h"
#include "peacemaker.h"

#include <string>

class Mccree : public Character
{
    public:
        Mccree(Gamestate *state, EntityPtr owner_);
        virtual ~Mccree() override;
        void render(Renderer *renderer, Gamestate *state) override;
        CharacterChildParameters constructCharacterChildParameters() override;
        Rect getcollisionrect(Gamestate *state) override;
        Rect getstandingcollisionrect(Gamestate *state) override;
        std::string getsprite(Gamestate *state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Mccree(*this));}
        EntityPtr getweapon(Gamestate *state) {return state->make_entity<Peacemaker>(state, EntityPtr(id));}
    protected:
    private:
};

#endif // MCCREE_H
