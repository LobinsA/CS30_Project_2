#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
/*
 added [#include "Actor.h"] @ 4/5/17 for usage of enum Misc in StudentWorld.cpp
 if this cause any problems in the future,
 we can replace enum Misc with strings instead,
 or find some other alternative. Let me know
 */

　
#include <string>
#include <vector>

　
// Students: Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

　
class Actor;
class DiggerMan;
class Dirt;

　
　
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
    void checkdroppedNugget(Item* actor, Protester* protester); // NEW!!! (edited)
    
    
    bool proximityCheck(Actor* actor, int distance);
    
    
    bool CPUspotsUser(Actor* CPU);
    bool CPUspotsUserfromAFar(Actor* CPU);
    
    
    void shootSquirtGun();
    void dropGoldNugget();
    
    void shout(int amount);
    
    ///////////////////////////////
    //// NEW FUNCTIONS ADDED /////
    /////////////////////////////
    bool atAnIntersection(Actor* CPU);
    int ProtesterRestTicks();
    int ProtesterStunTicks();

    //Arthur's Code
    void setDisplayText();
	int getBarrelCount() const { return m_BarrelCount; }
	void decBarrelCount() { m_BarrelCount--; }

private:
    std::vector<Actor*> m_actors;
    Dirt* m_land[VIEW_WIDTH][VIEW_HEIGHT];
    DiggerMan* m_user;

	//Arthur's Code
	int m_BarrelCount;
    
};

　
　
#endif // STUDENTWORLD_H_
