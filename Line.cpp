#include "Line.h"
using namespace Tools;

Line::Line()
{
}

Line::~Line()
{
}

void Line::process(Canvas &canvasMain, Canvas &canvasTemp,
    const MouseState &mouse, const PaintParameters &param)
{
    canvasTemp.clear();
    if (!m_leftClicked && mouse.isLeftPressed())
    {
        if (mouse.isOnCanvas())
        {
            m_lineStart = mouse.getPos();
            m_lineStart -= canvasMain.getPos();
            m_startOnCanvas = true;
        }
        else
        {
            m_startOnCanvas = false;
        }
    }
    else if (m_leftClicked && mouse.isLeftPressed() && m_startOnCanvas)
    {
        m_lineEnd = mouse.getPos();
        m_lineEnd -= canvasMain.getPos();
        drawLine(*canvasTemp.ptr(), m_lineStart, m_lineEnd, param);
    }
    else if (m_leftClicked && !mouse.isLeftPressed() && m_startOnCanvas)
    {
        drawLine(*canvasMain.ptr(), m_lineStart, m_lineEnd, param);
    }
    m_leftClicked = mouse.isLeftPressed();
}

void Line::drawLine(sf::RenderTarget &target,
    const sf::Vector2i &start, const sf::Vector2i &end,
    const PaintParameters &param)
{
    int h = target.getSize().y;
    // Оптимизация: при минимальной толщине можно рисовать одной линией
    if (param.width <= 1)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(start.x, h - start.y), param.color),
            sf::Vertex(sf::Vector2f(end.x,   h - end.y),   param.color)
        };
        target.draw(line, 2, sf::Lines);
    }
    else
    {
        float length = sqrtf(powf(end.x - start.x, 2)
            + powf(end.y - start.y, 2));

        // Оптимизация: При малом размере достаточно
        // рисовать только крайние точки
        if (length > 2.f)
        {
            float angle;
            // определение угла между осью Х и конечной точкой
            angle = atan2f(end.y - start.y, end.x - start.x);
            // инверсия угла (из-за особенностей вывода в текстуру)
            angle = -angle;
            // радианы в градусы
            angle *= 180.f / 3.141592f;

            sf::Vector2f size(length, param.width);
            sf::RectangleShape line;

            line.setOrigin(0, size.y / 2.f);
            line.setSize(size);
            line.setPosition(start.x, h - start.y);
            line.setFillColor(param.color);
            line.rotate(angle);
            target.draw(line);
        }
        sf::CircleShape point;
        point.setFillColor(param.color);
        point.setRadius(param.width / 2.f);
        point.setOrigin(param.width / 2.f, param.width / 2.f);
        point.setPosition(start.x, h - start.y);
        target.draw(point);
        point.setPosition(end.x,   h - end.y);
        target.draw(point);
    }
}
