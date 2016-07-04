#pragma once
#include "Line.h"

namespace Tools
{

// Инструмент "Кисточка"
class Brush :
    public Tool
{
public:
    Brush();
    ~Brush();

    void process(Canvas &canvasMain, Canvas &canvasTemp,
        const MouseState &mouse, const PaintParameters &param) override;

private:
    sf::Vector2i m_lineStart;
    sf::Vector2i m_lineEnd;
    bool m_leftClicked = false;
    bool m_startOnCanvas = false;
};

}; // namespace Tools
