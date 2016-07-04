#include "Rectangle.h"
using namespace Tools;

Rectangle::Rectangle()
{
}

Rectangle::~Rectangle()
{
}

void Rectangle::process(Canvas &canvasMain, Canvas &canvasTemp,
    const MouseState &mouse, const PaintParameters &param)
{
    canvasTemp.clear();
    if (!m_leftClicked && mouse.isLeftPressed())
    {
        if (mouse.isOnCanvas())
        {
            m_rectStart = mouse.getPos();
            m_rectStart -= canvasMain.getPos();
            m_startOnCanvas = true;
        }
        else
        {
            m_startOnCanvas = false;
        }
    }
    else if (m_leftClicked && mouse.isLeftPressed() && m_startOnCanvas)
    {
        m_rectEnd = mouse.getPos();
        m_rectEnd -= canvasMain.getPos();
        drawRectangle(*canvasTemp.ptr(), m_rectStart, m_rectEnd, param);
    }
    else if (m_leftClicked && !mouse.isLeftPressed() && m_startOnCanvas)
    {
        drawRectangle(*canvasMain.ptr(), m_rectStart, m_rectEnd, param);
    }
    m_leftClicked = mouse.isLeftPressed();
}

void Rectangle::drawRectangle(sf::RenderTarget &target,
    const sf::Vector2i &start, const sf::Vector2i &end,
    const PaintParameters &param)
{
    int h = target.getSize().y;
    sf::Vector2f size{ static_cast<float>(end.x - start.x),
                       static_cast<float>(end.y - start.y) };
    sf::RectangleShape rectangle;
    auto getCorrectWidth = [](const int width, const sf::Vector2f &size)
    {
        int minWidth = std::min(fabsf(size.x), fabsf(size.y)) / 2.f;
        return width > minWidth ? minWidth + 1 : width;
    };
    rectangle.setOutlineThickness(-getCorrectWidth(param.width, size));
    rectangle.setOutlineColor(param.color);
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    rectangle.setOrigin(size / 2.f);
    rectangle.setPosition(start.x + size.x / 2.f, h - start.y - size.y / 2.f);
    rectangle.setSize(size);
    target.draw(rectangle);
}
