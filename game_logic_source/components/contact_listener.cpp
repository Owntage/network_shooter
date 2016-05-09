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
		((ContactData*)(fixtureA->GetUserData()))->otherType = ((ContactData*)(fixtureB->GetUserData()))->type;
		((ContactData*)(fixtureA->GetUserData()))->otherActorID = ((ContactData*)(fixtureB->GetUserData()))->actorID;
		

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = false;
		((ContactData*)(fixtureB->GetUserData()))->otherActorID = ((ContactData*)(fixtureA->GetUserData()))->actorID;
		
		
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
		((ContactData*)(fixtureA->GetUserData()))->otherType = ((ContactData*)(fixtureB->GetUserData()))->type;
		((ContactData*)(fixtureA->GetUserData()))->otherActorID = ((ContactData*)(fixtureB->GetUserData()))->actorID;

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = true;
		((ContactData*)(fixtureB->GetUserData()))->otherActorID = ((ContactData*)(fixtureA->GetUserData()))->actorID;
	}
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) 
{
	
}

void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}