#include "Canvas.h"

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
}

void Canvas::setSize(const sf::Vector2i &size)
{
    m_canvas = std::make_unique<sf::RenderTexture>();
    m_canvas->create(size.x, size.y);
}

void Canvas::setPos(const sf::Vector2i &pos)
{
    m_pos = pos;
}

sf::Vector2i Canvas::getPos()
{
    return m_pos;
}

void Canvas::clear(const sf::Color &color)
{
    m_canvas->clear(color);
}

sf::RenderTexture *Canvas::ptr()
{
    return m_canvas.get();
}

void Canvas::draw(sf::RenderWindow &window)
{
    sf::Sprite sprite(m_canvas->getTexture());
    sprite.setPosition(m_pos.x, m_pos.y);
    window.draw(sprite);
}
