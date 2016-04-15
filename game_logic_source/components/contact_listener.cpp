#include "world.h"

void MyContactListener::addPair(std::pair<std::string, std::string> p)
{
	pairs.push_back(p);
}

void MyContactListener::clearPairs()
{
	pairs.clear();
}

void MyContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	if(fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		((ContactData*)(fixtureA->GetUserData()))->hit = true;
		((ContactData*)(fixtureA->GetUserData()))->release = false;
		((ContactData*)(fixtureA->GetUserData()))->hit_id = ((ContactData*)(fixtureB->GetUserData()))->id;
		

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = false;
		((ContactData*)(fixtureB->GetUserData()))->hit_id = ((ContactData*)(fixtureA->GetUserData()))->id;
		
		
	}
}

void MyContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if(fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		((ContactData*)(fixtureA->GetUserData()))->hit = true;
		((ContactData*)(fixtureA->GetUserData()))->release = true;
		((ContactData*)(fixtureA->GetUserData()))->hit_id = ((ContactData*)(fixtureB->GetUserData()))->id;

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = true;
		((ContactData*)(fixtureB->GetUserData()))->hit_id = ((ContactData*)(fixtureA->GetUserData()))->id;
	}
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) 
{
	
}

void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}