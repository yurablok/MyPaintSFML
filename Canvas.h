#pragma once

#include <memory>

#define SFML_STATIC
#include <SFML/Graphics.hpp>

class Canvas
{
public:
    Canvas();
    ~Canvas();

    void setSize(const sf::Vector2i &size);
    void setPos(const sf::Vector2i &pos);
    sf::Vector2i getPos();
    void clear(const sf::Color &color = sf::Color(0, 0, 0, 0));
    // Указатель на текстуру полотна
    sf::RenderTexture* ptr();
    // Вывод полотна в окно
    void draw(sf::RenderWindow &window);

private:
    std::unique_ptr<sf::RenderTexture> m_canvas;
    sf::Vector2i m_pos;
};
