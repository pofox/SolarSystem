#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>

const float width = 800.0f;
const float height = 600.0f;
const float pixelsperunit = 40.0f;

struct Planet
{
    b2Body* body;
    sf::CircleShape shape;
};

std::vector<Planet*> Planets;

sf::Vector2f box2sfml(b2Vec2 v)
{
    return sf::Vector2f(v.x * pixelsperunit + width / 2, height / 2 - v.y * pixelsperunit);
}

sf::Vector2f box2sfml(float x, float y)
{
    return sf::Vector2f(x * pixelsperunit + width / 2, height / 2 - y * pixelsperunit);
}

b2Vec2 sfml2box(sf::Vector2f v)
{
    return b2Vec2((v.x - width / 2) / pixelsperunit, (height / 2 - v.y) / pixelsperunit);
}

b2Vec2 sfml2box(float x, float y)
{
    return b2Vec2((x - width / 2) / pixelsperunit, (height / 2 - y) / pixelsperunit);
}

int main() {
    // SFML Setup
    sf::RenderWindow window(sf::VideoMode(width, height), "Box2D + SFML Test");
    window.setFramerateLimit(60);

    // Box2D World Setup
    b2Vec2 gravity(0.0f, -9.8f); // Gravity vector
    b2World world(gravity);

    // Ground Body (Static)
    b2BodyDef SunBodyDef;
    SunBodyDef.position.Set(0.0f, 0.0f); // Position at the bottom
    b2Body* SunBody = world.CreateBody(&SunBodyDef);
    
    b2CircleShape SunShape;
    SunShape.m_radius = 1;
    SunBody->CreateFixture(&SunShape, 0.0f); // Density = 0.0 for static objects

    // Dynamic Body (Falling Box)
    b2BodyDef dynamicBodyDef;
    dynamicBodyDef.type = b2_dynamicBody;
    dynamicBodyDef.position.Set(0.0f, 4.0f); // Starting position
    b2Body* dynamicBody = world.CreateBody(&dynamicBodyDef);

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = 0.5;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f; // Mass density
    fixtureDef.friction = 0.3f; // Friction
    dynamicBody->CreateFixture(&fixtureDef);

    // SFML Shapes for Rendering
    sf::CircleShape Sun(SunShape.m_radius * pixelsperunit);
    Sun.setFillColor(sf::Color::Yellow);
    Sun.setOrigin(pixelsperunit, pixelsperunit);
    Sun.setPosition(box2sfml(SunBodyDef.position)); // Bottom of the screen

    sf::CircleShape boxRect(dynamicBox.m_radius * pixelsperunit);
    boxRect.setFillColor(sf::Color::Green);
    boxRect.setOrigin(dynamicBox.m_radius * pixelsperunit, dynamicBox.m_radius * pixelsperunit); // Center origin for proper positioning

    // Main Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Box2D Simulation Step
        float timeStep = 1.0f / 60.0f; // 60 FPS
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        world.Step(timeStep, velocityIterations, positionIterations);

        // Update Box Position
        boxRect.setPosition(box2sfml(dynamicBody->GetPosition())); // Scale and translate to screen

        // Rendering
        window.clear();
        window.draw(Sun);
        window.draw(boxRect);
        window.display();
    }

    return 0;
}
