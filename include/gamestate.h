#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <unordered_map>
#include <memory>

#include "map.h"
#include "datastructures.h"

// FIXME: Circular references
class Player;
class Entity;

class Gamestate
{
    public:
        Gamestate();
        ~Gamestate();

        template<class EntityT, class ...Args>EntityPtr make_entity(Args&& ...args);
        PlayerPtr make_player();

        Entity* get(EntityPtr);
        Player* get(PlayerPtr);

        void update(double frametime);
        Gamestate* clone();

        std::unordered_map<int, std::unique_ptr<Entity>> entitylist;
        std::unordered_map<int, std::unique_ptr<Entity>> playerlist;

        double time;
        std::shared_ptr<Map> currentmap;
    protected:
    private:
        uint64_t entityidcounter;
        uint64_t playeridcounter;
};

#endif // GAMESTATE_H
