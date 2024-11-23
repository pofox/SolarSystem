#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#define G 6.6740831e-1

const float width = 800.0f;
const float height = 600.0f;
const float pixelsperunit = 40.0f;

struct Planet
{
    b2Body* body;
    sf::CircleShape shape;
    sf::VertexArray trail;
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

    float SUN_MASS = 100;

    bool clicked = false;
    
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
                if (!clicked)
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
                pfixtureDef.density = (float)(1+rand()%20)/10; // Mass density
                pfixtureDef.friction = 0.3f; // Friction
                p->body->CreateFixture(&pfixtureDef);
                b2Vec2 dir = p->body->GetPosition();
                dir = b2Vec2(sqrt(G * SUN_MASS / dir.Length()) * -dir.y / dir.Length(), sqrt(G * SUN_MASS / dir.Length()) * dir.x / dir.Length());
                if (rand() % 2)
                {
                    dir *= -1;
                }
                p->body->SetLinearVelocity(dir);
                p->body->ApplyForceToCenter(dir, true);
                p->shape.setPosition(mouse);
                p->shape.setRadius(pdynamicBox.m_radius * pixelsperunit);
                p->shape.setFillColor(sf::Color(rand()%255,rand()%255,rand()%255,255));
                p->shape.setOrigin(pdynamicBox.m_radius * pixelsperunit, pdynamicBox.m_radius * pixelsperunit);
                p->trail.setPrimitiveType(sf::LineStrip);
                Planets.push_back(p);
                }
                clicked = true;
            }
            else
            {
                clicked = false;
            }
        }

        std::vector<int> toDelete;
        std::vector<Planet*> toAdd;

        for (int i=0;i<Planets.size();i++)
        {
            if (i != Planets.size()-1)
            {
                for (int j=i+1;j<Planets.size();j++)
                {
                    b2Vec2 dir = Planets[j]->body->GetPosition() - Planets[i]->body->GetPosition();
                    if (dir.Length()-(Planets[i]->body->GetFixtureList()->GetShape()->m_radius+Planets[j]->body->GetFixtureList()->GetShape()->m_radius)<0.01)
                    {
                        toDelete.push_back(i);
                        toDelete.push_back(j);

                        Planet* p = new Planet;
                        b2BodyDef pBodyDef;
                        pBodyDef.type = b2_dynamicBody;
                        pBodyDef.position.Set((Planets[i]->body->GetPosition().x+Planets[j]->body->GetPosition().x)/2,(Planets[i]->body->GetPosition().y+Planets[j]->body->GetPosition().y) / 2); // Starting position
                        p->body = world.CreateBody(&pBodyDef);

                        b2CircleShape pdynamicBox;
                        pdynamicBox.m_radius = sqrt(pow(Planets[i]->body->GetFixtureList()->GetShape()->m_radius, 2) + pow(Planets[j]->body->GetFixtureList()->GetShape()->m_radius,2));

                        b2FixtureDef pfixtureDef;
                        pfixtureDef.shape = &pdynamicBox;
                        pfixtureDef.density = (Planets[i]->body->GetFixtureList()->GetDensity()+Planets[j]->body->GetFixtureList()->GetDensity())/2; // Mass density
                        pfixtureDef.friction = 0.3f; // Friction
                        p->body->CreateFixture(&pfixtureDef);

                        b2Vec2 vel = Planets[i]->body->GetLinearVelocity()+Planets[j]->body->GetLinearVelocity();
                        vel.x /= 2;
                        vel.y /= 2;
                        p->body->SetLinearVelocity(vel);
                        sf::Vector2f center = Planets[i]->shape.getPosition() + Planets[j]->shape.getPosition();
                        center.x /= 2;
                        center.y /= 2;
                        p->shape.setPosition(center);
                        p->shape.setRadius(pdynamicBox.m_radius * pixelsperunit);
                        sf::Color col1,col2;
                        col1 = Planets[i]->shape.getFillColor();
                        col2 = Planets[j]->shape.getFillColor();
                        col1.r /= 2;
                        col1.g /= 2;
                        col1.b /= 2;
                        col2.r /= 2;
                        col2.g /= 2;
                        col2.b /= 2;
                        col1 += col2;
                        p->shape.setFillColor(col1);
                        p->shape.setOrigin(pdynamicBox.m_radius * pixelsperunit, pdynamicBox.m_radius * pixelsperunit);
                        p->trail.setPrimitiveType(sf::LineStrip);
                        toAdd.push_back(p);
                    }
                    float force = dir.LengthSquared()!=0? G * Planets[i]->body->GetMass() * Planets[j]->body->GetMass() / dir.LengthSquared() : 0;
                    dir.Normalize();
                    dir *= force;
                    Planets[i]->body->ApplyForceToCenter(dir, true);
                    Planets[j]->body->ApplyForceToCenter(-dir, true);
                }
            }
            if (Planets[i]->body->GetPosition().Length() - SunShape.m_radius - Planets[i]->body->GetFixtureList()->GetShape()->m_radius < 0.01|| Planets[i]->body->GetPosition().Length()>20)
            {
                toDelete.push_back(i);
            }
            b2Vec2 dir = -Planets[i]->body->GetPosition();
            float force = G * Planets[i]->body->GetMass() * SUN_MASS / dir.LengthSquared();
            dir.Normalize();
            dir *= force;
            Planets[i]->body->ApplyForceToCenter(dir, true);
            sf::Vertex v(Planets[i]->shape.getPosition());
            v.color = Planets[i]->shape.getFillColor();
            if (Planets[i]->trail.getVertexCount() < 100)
            {
                Planets[i]->trail.append(v);
            }
            else {
                for (int j = 0; j < Planets[i]->trail.getVertexCount()-1; j++)
                {
                    Planets[i]->trail[j] = Planets[i]->trail[j + 1];
                }
                Planets[i]->trail[Planets[i]->trail.getVertexCount() - 1] = v;
            }
            for (int j = 0; j < Planets[i]->trail.getVertexCount() - 1; j++)
            {
                Planets[i]->trail[j].color.a = (j * 255) / (Planets[i]->trail.getVertexCount() - 1);
            }
        }
        for (int i = 0; i < toDelete.size(); i++)
        {
            if (toDelete[i]-i >= Planets.size()) break;
            world.DestroyBody(Planets[toDelete[i]-i]->body);
            delete(Planets[toDelete[i]-i]);
            Planets.erase(Planets.begin() + (toDelete[i] - i));
        }
        toDelete.clear();
        for (int i = 0; i < toAdd.size(); i++)
        {
            Planets.push_back(toAdd[i]);
        }
        toAdd.clear();

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
            window.draw(p->trail);
        }
        for (Planet* p : Planets)
        {
            window.draw(p->shape);
        }
        window.draw(Sun);
        window.display();
    }
    for (Planet* p : Planets)
    {
        world.DestroyBody(p->body);
        delete(p);
    }
    return 0;
}
