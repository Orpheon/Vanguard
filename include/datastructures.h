#ifndef DATASTRUCTURES_H_INCLUDED
#define DATASTRUCTURES_H_INCLUDED

#include <string>
#include <functional>
#include "networking/buffer.h"

class Gamestate;

enum HEROCLASS {ANA,
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
                SOLDIER76,
                SYMMETRA,
                TORBJORN,
                TRACER,
                WIDOWMAKER,
                WINSTON,
                ZARYA,
                ZENYATTA};

enum GAMETYPE { SERVER_ONLY,
                CLIENT_ONLY,
                SERVER_AND_CLIENT};

struct INPUT_CONTAINER
{
    bool LEFT;
    bool RIGHT;
    bool JUMP;
    bool CROUCH;
    bool PRIMARY_FIRE;
    bool SECONDARY_FIRE;
    bool RELOAD;
    bool ABILITY_1;
    bool ABILITY_2;
    bool ULTIMATE;

    void serialize(WriteBuffer *buffer)
    {
        uint16_t d = 0;
        d |= LEFT<<1;
        d |= RIGHT<<2;
        d |= JUMP<<3;
        d |= CROUCH<<4;
        d |= PRIMARY_FIRE<<5;
        d |= SECONDARY_FIRE<<6;
        d |= RELOAD<<7;
        d |= ABILITY_1<<8;
        d |= ABILITY_2<<9;
        d |= ULTIMATE<<10;
        buffer->write<uint16_t>(d);
    }

    void deserialize(ReadBuffer *buffer)
    {
        uint16_t d = buffer->read<uint16_t>();
        LEFT = d & 1<<1;
        RIGHT = d & 1<<2;
        JUMP = d & 1<<3;
        CROUCH = d & 1<<4;
        PRIMARY_FIRE = d & 1<<5;
        SECONDARY_FIRE = d & 1<<6;
        RELOAD = d & 1<<7;
        ABILITY_1 = d & 1<<8;
        ABILITY_2 = d & 1<<9;
        ULTIMATE = d & 1<<10;
    }
};

struct Health
{
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
    operator int() const { return id; }
    uint64_t id;
};

struct CharacterChildParameters
{
    double runpower = 0;
    EntityPtr weapon = 0;
    Health maxhp;
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

    Rect(double x_, double y_, double w_, double h_) : x(x_), y(y_), w(w_), h(h_) {}
    Rect offset(double x_, double y_) {return Rect(x+x_, y+y_, w, h);}
};

#endif // DATASTRUCTURES_H_INCLUDED
