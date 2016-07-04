#pragma once
#include "Tool.h"

namespace Tools
{

// Инструмент "Эллипс"
class Ellipse :
    public Tool
{
public:
    Ellipse();
    ~Ellipse();

    void process(Canvas &canvasMain, Canvas &canvasTemp,
        const MouseState &mouse, const PaintParameters &param) override;
    static void drawEllipse(sf::RenderTarget &target,
        const sf::Vector2i &start, const sf::Vector2i &end,
        const PaintParameters &param);

private:
    sf::Vector2i m_ellipseStart;
    sf::Vector2i m_ellipseEnd;
    bool m_leftClicked = false;
    bool m_startOnCanvas = false;
};

}; // namespace Tools
