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
    b2Vec2 gravity(0.0f, 0.0f); // Gravity vector
    b2World world(gravity);

    // Ground Body (Static)
    b2BodyDef SunBodyDef;
    SunBodyDef.position.Set(0.0f, 0.0f); // Position at the bottom
    b2Body* SunBody = world.CreateBody(&SunBodyDef);
    
    b2CircleShape SunShape;
    SunShape.m_radius = 1;
    SunBody->CreateFixture(&SunShape, 0.0f); // Density = 0.0 for static objects

    // SFML Shapes for Rendering
    sf::CircleShape Sun(SunShape.m_radius * pixelsperunit);
    Sun.setFillColor(sf::Color::Yellow);
    Sun.setOrigin(pixelsperunit, pixelsperunit);
    Sun.setPosition(box2sfml(SunBodyDef.position)); // Bottom of the screen

    // Main Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));
                mouse.x /= window.getSize().x;
                mouse.x *= width;
                mouse.y /= window.getSize().y;
                mouse.y *= height;
                Planet* p = new Planet;
                b2BodyDef pBodyDef;
                pBodyDef.type = b2_dynamicBody;
                pBodyDef.position.Set(sfml2box(mouse).x,sfml2box(mouse).y); // Starting position
                p->body = world.CreateBody(&pBodyDef);

                b2CircleShape pdynamicBox;
                pdynamicBox.m_radius = (float)(5+rand()%10)/20;

                b2FixtureDef pfixtureDef;
                pfixtureDef.shape = &pdynamicBox;
                pfixtureDef.density = 1.0f; // Mass density
                pfixtureDef.friction = 0.3f; // Friction
                p->body->CreateFixture(&pfixtureDef);
                p->shape.setPosition(mouse);
                p->shape.setRadius(pdynamicBox.m_radius * pixelsperunit);
                p->shape.setFillColor(sf::Color(rand()%255,rand()%255,rand()%255,255));
                p->shape.setOrigin(pdynamicBox.m_radius * pixelsperunit, pdynamicBox.m_radius * pixelsperunit);
                Planets.push_back(p);
            }
        }

        // Box2D Simulation Step
        float timeStep = 1.0f / 60.0f; // 60 FPS
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        world.Step(timeStep, velocityIterations, positionIterations);

        // Rendering
        window.clear();
        for (Planet* p : Planets)
        {
            p->shape.setPosition(box2sfml(p->body->GetPosition()));
            window.draw(p->shape);
        }
        window.draw(Sun);
        window.display();
    }
    for (Planet* p : Planets)
    {
        delete(p);
    }
    return 0;
}
