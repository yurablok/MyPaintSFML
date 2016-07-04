#include "MouseUtils.h"

MouseState::MouseState()
{
}

MouseState::~MouseState()
{
}

void MouseState::update(const sf::RenderWindow &window)
{
	m_pos = sf::Mouse::getPosition(window);
	m_left = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

sf::Vector2i MouseState::getPos() const
{
	return m_pos;
}

bool MouseState::isLeftPressed() const
{
	return m_left;
}

void MouseState::setOnCanvas(bool onCanvas)
{
	m_onCanvas = onCanvas;
}

bool MouseState::isOnCanvas() const
{
	return m_onCanvas;
}

bool isMouseInRect(const MouseState &mouse, const sf::FloatRect &rect)
{
	return isMouseInRect(mouse, sf::IntRect(
		rect.left, rect.top, rect.width, rect.height));
}

bool isMouseInRect(const MouseState &mouse, const sf::IntRect &rect)
{
    if (mouse.getPos().x >= rect.left
        && mouse.getPos().x < rect.left + rect.width
        && mouse.getPos().y >= rect.top
        && mouse.getPos().y < rect.top + rect.height)
	    return true;
    return false;
}
