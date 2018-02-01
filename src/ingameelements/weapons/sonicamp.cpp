
#include <gamestate.h>
#include <engine.h>
#include "ingameelements/weapons/sonicamp.h"

void Sonicamp::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Clipweapon::init(id_, state, owner_);
}

void Sonicamp::render(Renderer &renderer, Gamestate &state)
{
    // TODO
    ;
}

void Sonicamp::fireprimary(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Fired primary");
}

void Sonicamp::firesecondary(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Fired secondary");
}

void Sonicamp::wantfireprimary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and state.engine.isserver)
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Sonicamp::wantfiresecondary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and state.engine.isserver)
    {
        firesecondary(state);
        state.engine.sendbuffer.write<uint8_t>(SECONDARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}