#pragma once

#define SFML_STATIC
#include <SFML/Graphics.hpp>

class MouseState
{
public:
	MouseState();
	~MouseState();

	void update(const sf::RenderWindow &window);
	sf::Vector2i getPos() const;
	bool isLeftPressed() const;
	void setOnCanvas(bool onCanvas);
	bool isOnCanvas() const;

private:
	sf::Vector2i m_pos;
	bool m_left;
	bool m_onCanvas;
};

bool isMouseInRect(const MouseState &mouse, const sf::FloatRect &rect);
bool isMouseInRect(const MouseState &mouse, const sf::IntRect &rect);
