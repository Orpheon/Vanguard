#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <list>
#include <unordered_map>
#include <memory>

#include "ingameelements/map.h"
#include "datastructures.h"
#include "player.h"
#include "ingameelements/character.h"

// Circular reference
class Engine;

class Gamestate
{
    public:
        Gamestate(Engine *engine_);
        ~Gamestate();

        template<class EntityT, class ...Args>EntityPtr make_entity(Args&& ...args)
        {
            uint64_t id = entityidcounter++;
            entitylist[id] = std::unique_ptr<Entity>(new EntityT(id, std::forward<Args>(args)...));
            return EntityPtr(id);
        }

        template<class EntityT> EntityT* get(EntityPtr e)
        {
            if (e == 0 or entitylist.count(e.id) == 0 or entitylist[e.id]->destroyentity)
            {
                return 0;
            }
            return static_cast<EntityT*>(entitylist[e.id].get());
        }

        void update(double frametime);
        std::unique_ptr<Gamestate> clone();
        void interpolate(Gamestate *prevstate, Gamestate *nextstate, double alpha);
        EntityPtr addplayer();
        void serializesnapshot(WriteBuffer *buffer);
        void deserializesnapshot(ReadBuffer *buffer);
        void serializefull(WriteBuffer *buffer);
        void deserializefull(ReadBuffer *buffer);

        std::unordered_map<int, std::unique_ptr<Entity>> entitylist;
        std::list<EntityPtr> playerlist;

        // Make gamestate move-assigneable, so that " = " doesn't copy but move.
        Gamestate & operator=(Gamestate &&)=default;

        double time;
        std::shared_ptr<Map> currentmap;
        Engine *engine;
    protected:
    private:
        uint64_t entityidcounter;
};

#endif // GAMESTATE_H
