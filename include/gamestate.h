#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <unordered_map>
#include <memory>

#include "map.h"
#include "datastructures.h"
#include "player.h"
#include "character.h"

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
        std::unordered_map<int, std::unique_ptr<Player>> playerlist;

        // Make gamestate move-assigneable, so that " = " doesn't copy but move.
        Gamestate & operator=(Gamestate &&)=default;

        double time;
        std::shared_ptr<Map> currentmap;
    protected:
    private:
        uint64_t entityidcounter;
        uint64_t playeridcounter;
};

#endif // GAMESTATE_H
