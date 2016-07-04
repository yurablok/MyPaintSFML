#include "Ellipse.h"
using namespace Tools;

class EllipseShape :
    public sf::Shape
{
public:
    explicit EllipseShape(const sf::Vector2f &radius = sf::Vector2f(0, 0)) :
        m_radius(radius)
    {
        update();
    }

    void setRadius(const sf::Vector2f& radius)
    {
        m_radius = radius;
        update();
    }
    const sf::Vector2f& getRadius() const
    {
        return m_radius;
    }
    unsigned int getPointCount() const override
    {
        // Оптимизация: Чем меньше размер, тем меньше граней
        float count = std::max(fabsf(m_radius.x), fabs(m_radius.y)) / 3.f;
        if (count < 16.f)
            return 16;
        return count;
    }
    sf::Vector2f getPoint(unsigned int index) const override
    {
        static const float pi = 3.141592654f;

        float angle = index * 2.f * pi / getPointCount() - pi / 2.f;
        float x = std::cos(angle) * m_radius.x;
        float y = std::sin(angle) * m_radius.y;

        return sf::Vector2f(m_radius.x + x, m_radius.y + y);
    }

private:
    sf::Vector2f m_radius;
};

Ellipse::Ellipse()
{
}

Ellipse::~Ellipse()
{
}

void Ellipse::process(Canvas &canvasMain, Canvas &canvasTemp,
    const MouseState &mouse, const PaintParameters &param)
{
    canvasTemp.clear();
    if (!m_leftClicked && mouse.isLeftPressed())
    {
        if (mouse.isOnCanvas())
        {
            m_ellipseStart = mouse.getPos();
            m_ellipseStart -= canvasMain.getPos();
            m_startOnCanvas = true;
        }
        else
        {
            m_startOnCanvas = false;
        }
    }
    else if (m_leftClicked && mouse.isLeftPressed() && m_startOnCanvas)
    {
        m_ellipseEnd = mouse.getPos();
        m_ellipseEnd -= canvasMain.getPos();
        drawEllipse(*canvasTemp.ptr(), m_ellipseStart, m_ellipseEnd, param);
    }
    else if (m_leftClicked && !mouse.isLeftPressed() && m_startOnCanvas)
    {
        drawEllipse(*canvasMain.ptr(), m_ellipseStart, m_ellipseEnd, param);
    }
    m_leftClicked = mouse.isLeftPressed();
}

void Ellipse::drawEllipse(sf::RenderTarget &target,
    const sf::Vector2i &start, const sf::Vector2i &end,
    const PaintParameters &param)
{
    int h = target.getSize().y;
    sf::Vector2f size{ static_cast<float>(end.x - start.x),
                       -static_cast<float>(end.y - start.y) };
    EllipseShape ellipse;
    auto getCorrectWidth = [](const int width, const sf::Vector2f &size)
    {
        int minWidth = std::min(fabsf(size.x), fabsf(size.y)) / 2.f;
        return width > minWidth ? minWidth : width;
    };
    ellipse.setOutlineThickness(-getCorrectWidth(param.width, size));
    ellipse.setOutlineColor(param.color);
    ellipse.setFillColor(sf::Color(0, 0, 0, 0));
    ellipse.setPosition(start.x, h - start.y);
    ellipse.setRadius(size / 2.f);
    target.draw(ellipse);
}
