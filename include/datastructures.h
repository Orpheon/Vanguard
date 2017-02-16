#ifndef DATASTRUCTURES_H_INCLUDED
#define DATASTRUCTURES_H_INCLUDED

#include <string>
#include <functional>
#include "networking/buffer.h"

class Gamestate;

enum Heroclass {ANA,
                BASTION,
                DVA,
                GENJI,
                HANZO,
                JUNKRAT,
                LUCIO,
                MCCREE,
                MEI,
                MERCY,
                PHARAH,
                REAPER,
                REINHARDT,
                ROADHOG,
                SOMBRA,
                SOLDIER76,
                SYMMETRA,
                TORBJORN,
                TRACER,
                WIDOWMAKER,
                WINSTON,
                ZARYA,
                ZENYATTA};

enum EVENTTYPE {SERVER_SNAPSHOTUPDATE,
                SERVER_FULLUPDATE,
                CLIENT_INPUT,
                PLAYER_JOINED,
                PLAYER_LEFT,
                PLAYER_CHANGECLASS,
                PLAYER_SPAWNED,
                PLAYER_DIED,
                PRIMARY_FIRED,
                SECONDARY_FIRED,
                ABILITY1_USED,
                ABILITY2_USED,
                ULTIMATE_USED };

enum class ENTITYTYPE { PLAYER,
                        CHARACTER,
                        WEAPON,
                        PROJECTILE,
                        EXPLOSION };

enum Team { TEAM1,
            TEAM2,
            SPECTATOR };

struct ReducedInputContainer
{
    public:
        bool LEFT;
        bool RIGHT;
        bool CROUCH;

        void serialize(WriteBuffer *buffer)
        {
            uint8_t d = 0;
            d |= LEFT<<0;
            d |= RIGHT<<1;
            d |= CROUCH<<2;
            buffer->write<uint8_t>(d);
        }

        void deserialize(ReadBuffer *buffer)
        {
            uint8_t d = buffer->read<uint8_t>();
            LEFT = d & 1<<0;
            RIGHT = d & 1<<1;
            CROUCH = d & 1<<2;
        }

        void reset()
        {
            LEFT = false;
            RIGHT = false;
            CROUCH = false;
        }
};

struct InputContainer : public ReducedInputContainer
{
    bool JUMP;
    bool RELOAD;
    bool PRIMARY_FIRE;
    bool SECONDARY_FIRE;
    bool ABILITY_1;
    bool ABILITY_2;
    bool ULTIMATE;

    void serialize(WriteBuffer *buffer)
    {
        ReducedInputContainer::serialize(buffer);
        uint8_t d = 0;
        d |= JUMP<<0;
        d |= PRIMARY_FIRE<<1;
        d |= SECONDARY_FIRE<<2;
        d |= RELOAD<<3;
        d |= ABILITY_1<<4;
        d |= ABILITY_2<<5;
        d |= ULTIMATE<<6;
        buffer->write<uint8_t>(d);
    }

    void deserialize(ReadBuffer *buffer)
    {
        ReducedInputContainer::deserialize(buffer);
        uint8_t d = buffer->read<uint8_t>();
        JUMP = d & 1<<0;
        PRIMARY_FIRE = d & 1<<1;
        SECONDARY_FIRE = d & 1<<2;
        RELOAD = d & 1<<3;
        ABILITY_1 = d & 1<<4;
        ABILITY_2 = d & 1<<5;
        ULTIMATE = d & 1<<6;
    }

    void reset()
    {
        ReducedInputContainer::reset();
        JUMP = false;
        PRIMARY_FIRE = false;
        SECONDARY_FIRE = false;
        RELOAD = false;
        ABILITY_1 = false;
        ABILITY_2 = false;
        ULTIMATE = false;
    }

    ReducedInputContainer reduce()
    {
        ReducedInputContainer i;
        i.LEFT = LEFT;
        i.RIGHT = RIGHT;
        i.CROUCH = CROUCH;
        return i;
    }

    void update(ReducedInputContainer r)
    {
        LEFT = r.LEFT;
        RIGHT = r.RIGHT;
        CROUCH = r.CROUCH;
    }
};

struct Health
{
    Health(double n, double a, double s)
    {
        normal = n;
        armor = a;
        shields = s;
    }

    double normal;
    double armor;
    double shields;

    double total()
    {
        return normal + armor + shields;
    }
};

struct EntityPtr
{
    EntityPtr(uint64_t id_) : id(id_) {}
    EntityPtr() : id(0) {}
    operator int() const { return id; }
    uint64_t id;
};

struct CharacterChildParameters
{
    double runpower = 0;
    EntityPtr weapon = 0;
    Health maxhp;
    std::string characterfolder;
};

struct WeaponChildParameters
{
    int clipsize = 0;
    std::string characterfolder;
    std::function<void(Gamestate *state)> reloadfunction;
};

struct Rect
{
    double x;
    double y;
    double w;
    double h;

    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(double x_, double y_, double w_, double h_) : x(x_), y(y_), w(w_), h(h_) {}
    Rect offset(double x_, double y_) {return Rect(x+x_, y+y_, w, h);}
};

#endif // DATASTRUCTURES_H_INCLUDED
