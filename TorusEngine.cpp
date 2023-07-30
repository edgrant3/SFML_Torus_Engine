#include "TorusEngine.h"
#include <iostream>
#include <string>
#include <sstream>
const double pi = 3.14159265358979323846;

TorusEngine::TorusEngine() :
    TorusEngine(50)
{}

TorusEngine::TorusEngine(unsigned int numCircles) :
    dims{ sf::VideoMode::getDesktopMode().width,
          sf::VideoMode::getDesktopMode().height },
    drawHUD(true),
    numCircles(numCircles),
    circles(numCircles, dims.y * 0.02f, dims.y * 0.2f, dims, 1),
    pauseCircles(false),
    pullMouse(false),
    pushMouse(false),
    pushForce(40000),
    pullForce(100000)
{
    m_font.loadFromFile("DS-DIGIT.ttf");
    m_hud.setFont(m_font);
    m_hud.setCharacterSize(20);
    m_hud.setFillColor(sf::Color::White);

    m_Window.create(sf::VideoMode(dims.x, dims.y),
        "Circle Fun - Evan Grant SFML Project 0",
        sf::Style::Fullscreen);

    sf::Cursor cursor;
    m_Window.setMouseCursor(cursor);
    if (cursor.loadFromSystem(sf::Cursor::Cross))
        m_Window.setMouseCursor(cursor);
}

void TorusEngine::run()
{
    sf::Clock clock;
    sf::Time dt;
    float t = 10.f;
    int fps = 0;
    float hudPeriod = 0.5f;
    float prevT = 0.f;
    //circles.centerCircles();
    while (m_Window.isOpen())
    {
        dt = clock.restart();
        float dtSecs = dt.asSeconds();
        t += dtSecs;

        fps = (int)(1.f / dtSecs);

        // Update the HUD text
        if (t - prevT > hudPeriod)
        {
            prevT = t;
            std::stringstream ss;
            ss << "FPS: " << fps << " | circles: " << numCircles;
            m_hud.setString(ss.str());
        }
        
        input();
        update(dtSecs, t);
        draw();
    }
}

void TorusEngine::input()
{
    sf::Event event;
    while (m_Window.pollEvent(event))
    {
        // ALWAYS include this line (.exe will hang otherwise)
        if (event.type == sf::Event::Closed) { m_Window.close(); }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::F) {
                drawHUD = !drawHUD;
            }
            if (event.key.code == sf::Keyboard::P) {
                //printPositions();
            }
            if (event.key.code == sf::Keyboard::Space) {
                pauseCircles = !pauseCircles;
            }
            if (event.key.code == sf::Keyboard::Enter) {
                circles.centerCircles();
            }
            if (event.key.code == sf::Keyboard::Up) {
                numCircles += 50;
                regenerateCircles();
            }
            if (event.key.code == sf::Keyboard::Down) {

                numCircles = numCircles >= 50 ? numCircles - 50 : 0;
                regenerateCircles();
            }
            if (event.key.code == sf::Keyboard::Right) {
                numCircles += 1;
                regenerateCircles();
            }
            if (event.key.code == sf::Keyboard::Left) {
                numCircles = numCircles >= 1 ? numCircles - 1 : 0;
                regenerateCircles();
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    circles.togglePalette();
                }
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { m_Window.close(); }

    pullMouse = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    pushMouse = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void TorusEngine::update(float dtSecs, float tSecs)
{
    // Want circles to cross width of screen in 5s at max speed
    float speed = dims.x / 5.f;
    if (pullMouse) { magnetizeMouse(dtSecs, speed, true);  }
    if (pushMouse) { magnetizeMouse(dtSecs, speed, false); }

    if (!pauseCircles)
    {
        circles.periodicMovement(dtSecs, tSecs, speed);
    }
    
    circles.applyVelocity();
}

void TorusEngine::draw()
{
    m_Window.clear();
    for (auto &shape : circles.shapes)
    {
        wrapCircle(shape);
        m_Window.draw(shape);
    }
    if (drawHUD) { m_Window.draw(m_hud); }
    m_Window.display();
}


void TorusEngine::magnetizeMouse(float dtSecs, float speed, bool pull)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_Window);

    float dist2;
    sf::Vector2f dir;

    for (int i = 0; i < numCircles; i++)
    {
        sf::Vector2f shapePos = circles.shapes[i].getPosition();
        float rad = circles.shapes[i].getRadius();

        dir.x = shapePos.x + rad - mousePos.x;
        dir.y = shapePos.y + rad - mousePos.y;

        // X-axis check: is screen-wrapped mouse is closer than unwrapped mouse?
        float test = (dir.x > 0.f) ? dir.x - dims.x : dir.x + dims.x;
        if (std::abs(test) < std::abs(dir.x)) { dir.x = test; }

        // Y-axis check: is screen-wrapped mouse is closer than unwrapped mouse?
        test = (dir.y > 0.f) ? dir.y - dims.y : dir.y + dims.y;
        if (std::abs(test) < std::abs(dir.y)) { dir.y = test; }

        dist2 = dir.x * dir.x + dir.y * dir.y;
        float dist = std::sqrt(dist2);
        dir /= dist; // unit vector: shape -> mouse

        // Want to "normalize" force to the largest circle size
        float relative_rad = rad / circles.maxRad;
        float mass = relative_rad * relative_rad * pi;

        float force = pull? pullForce : pushForce;
        float magnitude = speed * dtSecs * (force / (mass * dist2));
        float xadj = dir.x;
        float yadj = dir.y;

        // Limit max speed when attracting circles (otherwise they glitch)
        if (pull) {
            float maxPullSpeed = 3000.f * dtSecs;
            magnitude = std::min(maxPullSpeed, magnitude);
            xadj = -xadj; yadj = -yadj;
        }

        xadj *= magnitude;
        yadj *= magnitude;

        circles.velocity[i] += { xadj, yadj };
    }
}

void TorusEngine::wrapCircle(sf::CircleShape &c)
{
    float rad = c.getRadius();
    float dia = 2.f * rad;

    sf::Vector2i duplicate({ 0, 0 });
    sf::Vector2f pos = c.getPosition();

    // Off screen left
    if (pos.x < -dia) { pos.x = dims.x - dia; }
    else if (pos.x < 0.f)
    {
        c.setPosition(dims.x + pos.x, pos.y);
        m_Window.draw(c);
        duplicate.x = 1;
    }

    // Off screen up
    if (pos.y < -dia) { pos.y = dims.y - dia; }
    else if (pos.y < 0.f)
    {
        c.setPosition(pos.x, dims.y + pos.y);
        m_Window.draw(c);
        duplicate.y = 1;
    }

    // Off screen right
    if (pos.x > dims.x) { pos.x = 0.f; }
    else if (pos.x > dims.x - dia)
    {
        c.setPosition(pos.x - dims.x, pos.y);
        m_Window.draw(c);
        duplicate.x = 2;
    }

    // Off screen down
    if (pos.y > dims.y) { pos.y = 0.f; }
    else if (pos.y > dims.y - dia)
    {
        c.setPosition(pos.x, pos.y - dims.y);
        m_Window.draw(c);
        duplicate.y = 2;
    }

    // Handle drawing on opposite corner if needed
    if (duplicate.x != 0 && duplicate.y != 0) 
    {
        if (duplicate.x == 1) 
        {
            c.setPosition((float)dims.x + pos.x, c.getPosition().y);
        }
        else 
        {
            c.setPosition(pos.x - (float)dims.x, c.getPosition().y);
        }
        if (duplicate.y == 1) 
        {
            c.setPosition(c.getPosition().x, (float)dims.y + pos.y);
        }
        else 
        {
            c.setPosition(c.getPosition().x, pos.y - (float)dims.y);
        }

        m_Window.draw(c);
    }

    // reset position for normal draw call following this func
    c.setPosition(pos);
}

void TorusEngine::printPositions()
{
    for (int i = 0; i < numCircles; i++)
    {
        sf::Vector2f pos = circles.shapes[i].getPosition();
        std::cout << "Circle " << i << " @ (" << pos.x << ", " << pos.y << ")" << std::endl;
    }
}

void TorusEngine::regenerateCircles()
{
    circles = CircleSet(numCircles, dims.y * 0.025f, dims.y * 0.2f, dims, circles.paletteToggle);
}