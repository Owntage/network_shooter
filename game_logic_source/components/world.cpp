#include "world.h"

World::World()
{
	b2Vec2 gravity(0,0);
	world = new b2World(gravity);
	contactListener = new MyContactListener;
	world->SetContactListener(contactListener);
}

World::~World()
{
	delete(world);
	delete(contactListener);
}

void World::update(double fps)
{
	world->Step(1.0/fps,6,2);
}

b2World* World::getWorld()
{
	return world;
}

b2Body* World::createStaticBody(double x, double y, double width, double height)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_staticBody;
	result = world->CreateBody(&bodyDef);
	b2PolygonShape shapeDef;
	shapeDef.SetAsBox(width / 2.0, height / 2.0);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

b2Body* World::createDynamicBody(double x, double y)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = false;
	result = world->CreateBody(&bodyDef);
	b2PolygonShape shapeDef;
	shapeDef.SetAsBox(0.5, 0.5);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

b2Body* World::createDynamicCircle(double x, double y, double r)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_dynamicBody;
	result = world->CreateBody(&bodyDef);
	b2CircleShape shapeDef;
	shapeDef.m_radius = r;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

void World::destroyBody(b2Body* body)
{
	world->DestroyBody(body);
}

void World::setFriction(b2Body* body, double friction)
{
	body->GetFixtureList()->SetFriction(friction);
	body->SetLinearDamping(friction*DEFAULT_FRICTION);
}
void World::setDensity(b2Body* body, double density)
{
	body->GetFixtureList()->SetDensity(density);
}
void World::setRestitution(b2Body* body, double restitution)
{
	body->GetFixtureList()->SetRestitution(restitution);
}
void World::setContactData(b2Body* body, ContactData& contactData)
{
	body->GetFixtureList()->SetUserData(&contactData);
}