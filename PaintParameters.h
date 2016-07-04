#pragma once

#define SFML_STATIC
#include <SFML/Graphics.hpp>

struct PaintParameters
{
    sf::Color color = sf::Color::Black;
    int width = 1;
};
