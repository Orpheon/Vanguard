#include "ingameelements/trail.h"

#include <SFML/Graphics/RectangleShape.hpp>

void Trail::init(uint64_t id_, Gamestate &state, sf::Color color_, double x1_, double y1_, double x2_, double y2_, double duration)
{
    Entity::init(id_);

    color = color_;
    x1 = x1_;
    y1 = y1_;
    x2 = x2_;
    y2 = y2_;
    countdown.init(duration, std::bind(&Trail::destroy, this, std::placeholders::_1), true);
}

void Trail::beginstep(Gamestate &state, double frametime)
{
    countdown.update(state, frametime);
}

void Trail::render(Renderer &renderer, Gamestate &state)
{
    sf::RectangleShape line(sf::Vector2f(hypot(x2-x1, y2-y1), 1));
    line.setRotation(std::atan2(y2-y1, x2-x1)*180.0/3.1415);
    line.setPosition(x1, y1);
    line.setFillColor(color);
    renderer.background.draw(line);
}

void Trail::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Trail &prev_e = reinterpret_cast<Trail&>(prev_entity);
    Trail &next_e = reinterpret_cast<Trail&>(next_entity);

    countdown.interpolate(next_e.countdown, prev_e.countdown, alpha);
}
