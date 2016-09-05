#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <unordered_map>
#include <memory>

#include "map.h"
#include "entityptr.h"

// FIXME: Circular references
class Player;
class Entity;

class Gamestate
{
    public:
        Gamestate();
        ~Gamestate();
        template<class EntityT, class ...Args>EntityPtr<EntityT> make_entity(Args&& ...args);
        PlayerPtr make_player();

        void update(double frametime);
        Gamestate* clone();

        std::unordered_map<int, Entity*> entitylist;
        std::unordered_map<int, Player*> playerlist;

        double time;
        std::shared_ptr<Map> currentmap;
    protected:
    private:
        uint64_t entityidcounter;
        uint64_t playeridcounter;
};

#endif // GAMESTATE_H
