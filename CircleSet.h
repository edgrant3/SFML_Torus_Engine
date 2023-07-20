#pragma once

#include <SFML/Graphics.hpp>
#include <array>

class CircleSet
{
private:

	unsigned int numCircles;
	//uint8_t numCircles;
	uint8_t paletteToggle;
	sf::Vector2f dims;
	
	float minRad;
	float maxRad;

	std::vector<sf::CircleShape> shapes;
	void createCircles();

	std::array<std::vector<sf::Color>, 5> palettes;
	void createPalettes();
	void updateColors();

	std::vector<sf::Vector2f> velocity;
	std::vector<sf::Vector2f> period;
	std::vector<sf::Vector2i> direction;
	void generatePeriods(float maxTimeSecs);
	
public:
	CircleSet(unsigned int numCircles, float minRad, float maxRad, const sf::Vector2f& dims, uint8_t paletteToggle);

	void periodicMovement(float dtSecs, float tSecs, float speed);
	void applyVelocity();
	void togglePalette();
	void centerCircles();

	friend class TorusEngine;
};