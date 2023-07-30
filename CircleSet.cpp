#include "CircleSet.h"
#include <iostream>
//#include <cstdlib>


const std::array<sf::Color, 7> HEAT_PALETTE{ sf::Color(150,   0,   0), sf::Color(200,  20,  0),
									   sf::Color(220, 100,   0), sf::Color(240, 150,  0),
									   sf::Color(255, 200,  20), sf::Color(255, 225, 60),
									   sf::Color(255, 255, 255) };

const std::array<sf::Color, 7> PRIMARY_PALETTE{ sf::Color(255,   0,   0), sf::Color(0, 255,   0), // red, green
										  sf::Color(0,   0, 255), sf::Color(255,   0, 255), // blue, magenta
										  sf::Color(255, 255,   0), sf::Color(0, 255, 255), // yellow, cyan
										  sf::Color(255, 255, 255) };                          // white

const std::array<sf::Color, 7> GREY_PALETTE{ sf::Color(25,  25,  25), sf::Color(40,  40,  40),
									   sf::Color(75,  75,  75), sf::Color(125, 125, 125),
									   sf::Color(175, 175, 175), sf::Color(225, 225, 225),
									   sf::Color(255, 255, 255) };

const std::array<sf::Color, 7> RAINBOW_PALETTE{ sf::Color(255,  0,  0), sf::Color(255,  127,  0),
									   sf::Color(255,  255,  0), sf::Color(0, 255, 0),
									   sf::Color(0, 127, 127), sf::Color(0, 0, 225),
									   sf::Color(180, 0, 255) };

const std::array<sf::Color, 7> GABBY_PALETTE{ sf::Color(81,51,104),
											  sf::Color(81,51,104),
											  sf::Color(106,102,163),
											  sf::Color(132,169,192),
											  sf::Color(179,203,185),
											  sf::Color(234,229,196),
											  sf::Color(234,229,196) };


CircleSet::CircleSet(unsigned int numCircles, float minRad, float maxRad, const sf::Vector2f& dims, uint8_t paletteToggle) :
	numCircles(numCircles),
	dims(dims),
	paletteToggle(paletteToggle),
	minRad(minRad),
	maxRad(maxRad)
{
	//std::cout << "Creating " << numCircles << " circles!" << std::endl;
	createPalettes();
	createCircles();
	generatePeriods(30.f);
}

void CircleSet::createCircles()
{
	float xMin = std::sqrt(minRad);
	float xMax = std::sqrt(maxRad);
	float dx   = (xMax - xMin) / (float)numCircles;

	// 0th element is always largest circle
	float x = xMax; 
	float startPos = 0.f;
	for (int i = 0; i < numCircles; ++i)
	{
		shapes.push_back(sf::CircleShape(x * x));		
		shapes[i].setFillColor(palettes[paletteToggle - 1][i]);
		shapes[i].setPosition({ (float)(std::rand() % (int)dims.x), (float)(std::rand() % (int)dims.y) });
		velocity.push_back({ 0.f, 0.f });
		x -= dx;
	}
}

sf::Color lerpColor(sf::Color left, sf::Color right, float t)
{
	return sf::Color((sf::Uint8)((float)left.r * (1.f - t) + (float)right.r * t),
					 (sf::Uint8)((float)left.g * (1.f - t) + (float)right.g * t),
					 (sf::Uint8)((float)left.b * (1.f - t) + (float)right.b * t) );
}

void CircleSet::createPalettes()
{
	float dx = 7.f / numCircles;
	float  x = 0.f;
	for (int i = 0; i < numCircles; ++i)
	{
		int integral = std::floor(x);
		float t = x - (float)integral;

		// Color LERP
		sf::Color heatCol    = HEAT_PALETTE.back();
		sf::Color primaryCol = PRIMARY_PALETTE.back();
		sf::Color greyCol    = GREY_PALETTE.back();
		sf::Color rainbowCol = RAINBOW_PALETTE.back();
		sf::Color gabCol     = GABBY_PALETTE.back();

		
		if (integral < HEAT_PALETTE.size() - 1)
		{ 
			heatCol    = lerpColor(HEAT_PALETTE[integral],    HEAT_PALETTE[integral + 1],    t);
			primaryCol = lerpColor(PRIMARY_PALETTE[integral], PRIMARY_PALETTE[integral + 1], t);
			greyCol    = lerpColor(GREY_PALETTE[integral],    GREY_PALETTE[integral + 1],    t);
			rainbowCol = lerpColor(RAINBOW_PALETTE[integral], RAINBOW_PALETTE[integral + 1], t);
			gabCol     = lerpColor(GABBY_PALETTE[integral],   GABBY_PALETTE[integral + 1],   t);
		}
		else
		{
			heatCol    = lerpColor(HEAT_PALETTE[integral],    heatCol,    t);
			primaryCol = lerpColor(PRIMARY_PALETTE[integral], primaryCol, t);
			greyCol    = lerpColor(GREY_PALETTE[integral],    greyCol,    t);
			rainbowCol = lerpColor(RAINBOW_PALETTE[integral], rainbowCol, t);
			gabCol     = lerpColor(GABBY_PALETTE[integral],   gabCol,     t);

		}

		palettes[0].push_back(heatCol);
		palettes[1].push_back(primaryCol);
		palettes[2].push_back(greyCol);
		palettes[3].push_back(rainbowCol);
		palettes[4].push_back(gabCol);

		x += dx;
	}
}

void CircleSet::generatePeriods(float maxTimeSecs)
{
	unsigned int denom = 1000;
	for (int i = 0; i < numCircles; ++i)
	{
		int randx = std::rand() % denom + denom / 50;
		int randy = std::rand() % denom + denom / 50;

		direction.push_back({ randx % 2, randy % 2 });

		float periodx = maxTimeSecs * (float)randx / (float)denom;
		float periody = maxTimeSecs * (float)randy / (float)denom;

		period.push_back({ 1.f / periodx, 
						   1.f / periody });
	}
}

void CircleSet::periodicMovement(float dtSecs, float tSecs, float speed)
{
	short int dir = 1;
	for (int i = 0; i < numCircles; ++i)
	{
		float xVel = std::cos(period[i].x * tSecs);
		float yVel = std::cos(period[i].y * tSecs);

		float norm = std::sqrt(xVel * xVel + yVel * yVel);

		xVel = dtSecs * speed * xVel / norm;
		yVel = dtSecs * speed * yVel / norm;

		velocity[i].x += direction[i].x ? xVel : -xVel;
		velocity[i].y += direction[i].y ? yVel : -yVel;
	}
}


void CircleSet::updateColors()
{
	std::vector<sf::Color> palette = palettes[paletteToggle - 1];

	for (int i = 0; i < numCircles; ++i) 
	{
		shapes[i].setFillColor(palette[i]);
	}
}

void CircleSet::togglePalette()
{
	paletteToggle = paletteToggle++ % palettes.size();
	updateColors();
}

void CircleSet::applyVelocity()
{
	for (int i = 0; i < numCircles; ++i)
	{
		sf::Vector2f pos = shapes[i].getPosition();
		//if (isnan(pos.x) || isnan(pos.y))
		//{
		//	pos = { 0.f, 0.f };
		//}
		if (isnan(velocity[i].x) || isnan(velocity[i].y))
		{
			velocity[i] = { 0.f, 0.f };
		}

		shapes[i].setPosition(pos + velocity[i]);
		velocity[i] = { 0.f, 0.f };
	}
}

void CircleSet::centerCircles()
{
	sf::Vector2f halfDims = dims / 2.f;

	for (auto &shape : shapes)
	{
		float rad = shape.getRadius();
		shape.setPosition({halfDims.x - rad, halfDims.y - rad});
	}
}