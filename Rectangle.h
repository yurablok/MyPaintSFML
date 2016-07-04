#pragma once
#include "Tool.h"

namespace Tools
{

// Инструмент "Прямоугольник"
class Rectangle :
    public Tool
{
public:
    Rectangle();
    ~Rectangle();

    void process(Canvas &canvasMain, Canvas &canvasTemp,
        const MouseState &mouse, const PaintParameters &param) override;
    static void drawRectangle(sf::RenderTarget &target,
        const sf::Vector2i &start, const sf::Vector2i &end,
        const PaintParameters &param);

private:
    sf::Vector2i m_rectStart;
    sf::Vector2i m_rectEnd;
    bool m_leftClicked = false;
    bool m_startOnCanvas = false;
};

}; // namespace Tools
