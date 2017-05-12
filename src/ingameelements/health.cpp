
#include "ingameelements/health.h"
#include "gamestate.h"

Health::Health(double n, double a, double s)
{
    max_normal = n;
    max_armor = a;
    max_shields = s;
    normal = max_normal;
    armor = max_armor;
    shields = max_shields;
    damaged.init(3);
}

double Health::damage(double amount)
{
    double prevhp = total();

    if (lucioshields > 0)
    {
        amount -= lucioshields;
        lucioshields = 0;
        if (amount < 0)
        {
            lucioshields -= amount;
            return prevhp - total();
        }
    }
    if (symshields > 0)
    {
        amount -= symshields;
        symshields = 0;
        if (amount < 0)
        {
            symshields -= amount;
            return prevhp - total();
        }
    }
    if (shields > 0)
    {
        amount -= shields;
        shields = 0;
        if (amount < 0)
        {
            shields -= amount;
            return prevhp - total();
        }
    }
    if (armor > 0 or torbarmor > 0)
    {
        // Apply armor damage reduction
        amount -= std::min(5.0, amount/2.0);
    }
    if (torbarmor > 0)
    {
        amount -= torbarmor;
        torbarmor = 0;
        if (amount < 0)
        {
            torbarmor -= amount;
            return prevhp - total();
        }
    }
    if (armor > 0)
    {
        amount -= armor;
        armor = 0;
        if (amount < 0)
        {
            armor -= amount;
            return prevhp - total();
        }
    }
    normal = std::max(0.0, normal-amount);
    return prevhp - total();
}

double Health::heal(double amount)
{
    double prevhp = total();

    if (normal < max_normal)
    {
        amount -= max_normal - normal;
        normal = std::min(max_normal, max_normal+amount);
        if (amount < 0)
        {
            return total() - prevhp;
        }
    }
    if (armor < max_armor)
    {
        amount -= max_armor - armor;
        armor = std::min(max_armor, max_armor+amount);
        if (amount < 0)
        {
            return total() - prevhp;
        }
    }
    if (shields < max_shields)
    {
        amount -= max_shields - shields;
        shields = std::min(max_shields, max_shields+amount);
    }
    return total() - prevhp;
}

void Health::update(Gamestate &state, double frametime)
{
    damaged.update(state, frametime);
    if (not damaged.active)
    {
        double amount = SHIELD_RECHARGE_RATE * frametime;
        if (shields < max_shields)
        {
            amount -= max_shields - shields;
            shields = std::min(max_shields, max_shields+amount);
        }
        if (amount > 0 and symshields < max_symshields)
        {
            amount -= max_symshields - symshields;
            symshields = std::min(max_symshields, max_symshields+amount);
        }
    }
}

void Health::interpolate(Health &prev_health, Health &next_health, double alpha)
{
    normal = prev_health.normal + alpha * (next_health.normal - prev_health.normal);
    armor = prev_health.armor + alpha * (next_health.armor - prev_health.armor);
    shields = prev_health.shields + alpha * (next_health.shields - prev_health.shields);
    torbarmor = prev_health.torbarmor + alpha * (next_health.torbarmor - prev_health.torbarmor);
    symshields = prev_health.symshields + alpha * (next_health.symshields - prev_health.symshields);
    lucioshields = prev_health.lucioshields + alpha * (next_health.lucioshields - prev_health.lucioshields);
    max_normal = prev_health.max_normal + alpha * (next_health.max_normal - prev_health.max_normal);
    max_armor = prev_health.max_armor + alpha * (next_health.max_armor - prev_health.max_armor);
    max_shields = prev_health.max_shields + alpha * (next_health.max_shields - prev_health.max_shields);
    max_symshields = prev_health.max_symshields + alpha * (next_health.max_symshields - prev_health.max_symshields);
    damaged.interpolate(prev_health.damaged, next_health.damaged, alpha);
}

double Health::total()
{
    return normal + armor + shields + torbarmor + symshields + lucioshields;
}

double Health::max()
{
    return max_normal + max_armor + max_shields + max_symshields + torbarmor + lucioshields;
}

void Health::fill()
{
    normal = max_normal;
    armor = max_armor;
    shields = max_shields;
    symshields = max_symshields;
}