#ifndef DATASTRUCTURES_H_INCLUDED
#define DATASTRUCTURES_H_INCLUDED

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

#endif // DATASTRUCTURES_H_INCLUDED
