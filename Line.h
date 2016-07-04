#pragma once
#include "Tool.h"

namespace Tools
{

// Инструмент "Линия"
class Line :
    public Tool
{
public:
    Line();
    ~Line();

    void process(Canvas &canvasMain, Canvas &canvasTemp,
        const MouseState &mouse, const PaintParameters &param) override;
    static void drawLine(sf::RenderTarget &target,
        const sf::Vector2i &start, const sf::Vector2i &end,
        const PaintParameters &param);

private:
    sf::Vector2i m_lineStart;
    sf::Vector2i m_lineEnd;
    bool m_leftClicked = false;
    bool m_startOnCanvas = false;
};

}; // namespace Tools
