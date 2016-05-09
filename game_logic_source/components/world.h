#ifndef WORLD_H
#define WORLD_H

#include <Box2D/Box2D.h>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "../gui_source/singleton.h"

#define DEFAULT_FRICTION 12.0

class ContactData
{
public:
	ContactData()
	{
		hit = false;
		release = false;
	}
	bool hit;
	bool release;
	std::string type;
	std::string otherType;
	int actorID;
	int otherActorID;
};


class MyContactListener: public b2ContactListener
{
private:
	std::vector<std::pair<std::string, std::string>> pairs;
public:
	void addPair(std::pair<std::string, std::string> p);
	void clearPairs();
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

class World: public Singleton<World>
{
private:
	b2World* world;
	b2ContactListener* contactListener;
	float time;
public:
	World();
	~World();
	void update(double fps);
	float getTime();
	b2World* getWorld();
	b2Body* createStaticBody(double x, double y, double width = 0.5, double height = 0.5);
	b2Body* createDynamicBody(double x, double y);
	b2Body* createDynamicCircle(double x, double y, double r = 0.5);
	void destroyBody(b2Body* body);
	static void setFriction(b2Body* body, double friction);
	static void setDensity(b2Body* body, double density);
	static void setRestitution(b2Body* body, double restitution);
	static void setContactData(b2Body* body, ContactData& contactData);
};

#endif
