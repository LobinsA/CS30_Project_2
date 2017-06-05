#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"

#include <string>
#include <vector>

// Students: Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class DiggerMan;
class Dirt;

struct node {
	int m_x;
	int m_y;
	GraphObject::Direction m_dir;
	bool m_visited;
	int m_stepCount;
	void updateMin(int x, int y, GraphObject::Direction d, int stepcount)
	{
		m_x = x;
		m_y = y;
		m_stepCount = stepcount;
		m_dir = d;
	}
	int getMinCount() { return m_stepCount; }
	node(int x, int y) : m_x(x), m_y(y), m_dir(GraphObject::none), m_visited(false), m_stepCount(0) { }
	node(int x, int y, int count) : m_x(x), m_y(y), m_dir(GraphObject::none), m_visited(false), m_stepCount(count) {}
	node() : m_x(0), m_y(0), m_dir(GraphObject::none), m_visited(false), m_stepCount(0) {}
};
class StudentWorld : public GameWorld
{
public:

	StudentWorld(std::string assetDir);
	~StudentWorld();


	virtual int init();
	virtual int move();
	virtual void cleanUp();


	void refillField();
	void buildMineShaft();
	void placeObjects();
	void distributeObject(Actor::Misc obj);

	void removeDead();


	void insertActor(Actor* entry);
	bool removeDirt(Actor* p);


	void updateCoord(GraphObject::Direction dir, int& x, int& y, int distance = 1);
	bool outOfBounds(const int& x, const int& y) const;
	bool BlockedByBoulder(Actor* OneSelf, int x, int y) const;


	bool LayerOfDirt4x1(int x, int y) const;
	bool LayerOfDirt4x4(int x, int y) const;


	bool withinRad(double x1, double y1, Actor* P2, double radius) const;
	bool withinRad(Actor* P1, Actor* P2, double radius) const;



	bool annoyCharactersNearby(Actor* p, int radius, int points, Actor::Misc object);


	void scanForItems();
	void checkItemPickup(Item* actor);
	void checkdroppedNugget(Item* actor, Protester* protester);


	bool proximityCheck(Actor* actor, int distance);


	bool CPUspotsUser(Actor* CPU);
	bool CPUspotsUserfromAFar(Actor* CPU);


	void shootSquirtGun();
	void dropGoldNugget();

	void shout(int amount);

	bool atAnIntersection(Actor* CPU);
	int ProtesterRestTicks();
	int ProtesterStunTicks();

	//Arthur's Code
	void setDisplayText();
	int getBarrelCount() const { return m_BarrelCount; }
	void decBarrelCount() { m_BarrelCount--; }
	void incrProtesterCount() { m_protesterCount++; }
	void decProtesterCount() { m_protesterCount--; }
	void BFS(node src, Actor* dest);
	void followShortestPath(Protester* CPU, node dest);
	void buildNodeMaze();
	void updateNodeMaze(Actor* actor);
	void destroyNodeMaze();
	void resetNodeMaze();
private:
	std::vector<Actor*> m_actors;
	Dirt* m_land[VIEW_WIDTH][VIEW_HEIGHT];
	DiggerMan* m_user;

	//Arthur's Code
	int m_BarrelCount;
	node* NodeMaze[VIEW_WIDTH][VIEW_HEIGHT];


	// David's code
	int m_protesterSpawnRest;
	int m_protesterCount;
};

#endif // STUDENTWORLD_H_
