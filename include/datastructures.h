#ifndef DATASTRUCTURES_H_INCLUDED
#define DATASTRUCTURES_H_INCLUDED

#include <string>

enum HEROCLASS {BASTION,
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

struct INPUT_CONTAINER
{
    bool LEFT;
    bool RIGHT;
    bool JUMP;
    bool CROUCH;
    bool PRIMARY_FIRE;
    bool SECONDARY_FIRE;
    bool ABILITY_1;
    bool ABILITY_2;
    bool ULTIMATE;
};

struct EntityPtr
{
    EntityPtr(uint64_t id_) : id(id_) {}
    operator int() const { return id; }
    uint64_t id;
};

struct PlayerPtr
{
    PlayerPtr(uint64_t id_) : id(id_) {}
    operator int() const { return id; }
    uint64_t id;
};

struct CharacterChildParameters
{
    std::string walkanimpath;
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
