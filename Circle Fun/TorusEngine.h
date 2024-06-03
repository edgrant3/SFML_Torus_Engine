#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "CircleSet.h"

class TorusEngine
{
private:

	sf::Vector2f dims;
	sf::RenderWindow m_Window;
	sf::Cursor cursor;

	sf::Font m_font;
	sf::Text m_hud;
	bool drawHUD;

	int32_t m_timeInterpIdx;
	float m_timeInflation;

	unsigned int numCircles;
	//uint8_t numCircles;
	CircleSet circles;
	std::vector<sf::Vector2f> velocity;

	bool pauseCircles;
	bool  pullMouse;
	bool  pushMouse;
	float pushForce;
	float pullForce;

	void input();
	void update(float dtSecs, float tSecs);
	void draw();

	void magnetizeMouse(float dtSecs, float speed, bool pull);
	void wrapCircle(sf::CircleShape &c);

	void printPositions();
	void regenerateCircles();

public:
	TorusEngine();
	TorusEngine(unsigned int numCircles);

	void run();
};