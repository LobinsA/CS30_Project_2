#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm> // added @ 4/20 for use of the max function
#include <queue>

/*
[] Notes on Actor identifiers
----------------------------
virtual bool canBlockCharacters() {return true}:
means Actor is a Boulder
virtual bool canCollectItems() {return true}:
means Actor is a Character (DiggerMan or Protester)


These are in use when iterating through a vector
To find Boulders within the vector...
for (int i = 0; i < m_actors.size(); i++)
if (m_actors[i]->canBlockCharacters())
{ // do something with the boulder }
*/


// Students: Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
	/* Actor identifiers (for misc objects) */
	enum Misc { SONARKIT, BARRELOFOIL, SQUIRT, BOULDER, GOLDNUGGET, DROPPEDNUGGET, WATERPOOL };


	Actor(StudentWorld* world, int imgID, int x_pos, int y_pos,
		Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imgID, x_pos, y_pos, dir, size, depth), m_world(world), m_alive(true) {}
	virtual ~Actor() {}


	virtual void doSomething() = 0;
	bool makeMove();


	StudentWorld* accessToWorld() const { return m_world; }
	virtual void annoy(int amount) { return; }


	bool isAlive() const { return m_alive; }
	void setDead() { m_alive = false; }


	/* Actor identifiers */
	virtual bool canBlockCharacters() { return false; }
	virtual bool canCollectItems() { return false; }
	virtual bool hasCellPhoneTracker() { return false; }
	virtual bool coordinateCheck(int x, int y);


private:
	StudentWorld *m_world;
	bool m_alive;
};



class Character : public Actor
{
public:
	Character(StudentWorld* world, int imgID, int x_pos, int y_pos, int hp,
		Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(world, imgID, x_pos, y_pos, dir, size, depth), m_HP(hp)
	{
		setVisible(true);
	}

	virtual ~Character() {}
	int getTotalHP() const { return m_HP; }
	void subtractFromHP(int amount) { m_HP -= amount; }
	virtual bool canCollectItems() { return true; }

private:
	int m_HP;


};


class DiggerMan : public Character
{
public:
	DiggerMan(StudentWorld* world)
		: Character(world, IMID_PLAYER, 30, 60, 10), m_squirts(5), m_goldnuggets(0), m_sonarcharges(1) {}
	virtual ~DiggerMan() {}


	virtual void doSomething();
	virtual void annoy(int amount);


	int getSquirts() { return m_squirts; }
	int getGoldNuggets() { return m_goldnuggets; }
	int getSonarCharges() { return m_sonarcharges; }


	void gainSquirts() { m_squirts += 5; }
	void gainSonarCharge() { m_sonarcharges++; }
	void gainGoldNugget() { m_goldnuggets++; }

	virtual bool coordinateCheck(int x, int y);
private:
	int m_squirts;
	int m_goldnuggets;
	int m_sonarcharges;
};




/*
[] Info on actions on Protesters
--------------------------------
For causing a Regular Protester to give up: 100 points
For causing a Hardcore Protester to give up: 250 points

For bribing a Regular Protester with a Gold Nugget: 25 points
For bribing a Hardcore Protester with a Gold Nugget: 50 points

For causing a Boulder to bonk a Protester: 500 points
*/

class Protester : public Character  /* UNDER CONSTRUCTION */
{
public:
	Protester(StudentWorld* world, int imgID, int hp, int ticks)
		: Character(world, imgID, 60, 60, hp, left)
	{
		m_leave = false;
		m_shoutRecovery = 0;
		m_rightAngleTurnRecovery = 0;
		m_rest = ticks;
		m_distanceInCurDir = rand() % 53 + 8;

		setVisible(true);
	}

	Protester(StudentWorld* world, int x, int y, int imgID, int hp, int ticks)
		: Character(world, imgID, x, y, hp, left)
	{
		m_leave = false;
		m_shoutRecovery = 0;
		m_rightAngleTurnRecovery = 0;
		m_rest = ticks;
		m_distanceInCurDir = rand() % 53 + 8;

		setVisible(true);
	}

	virtual ~Protester() {}
	virtual void doSomething();
	virtual void annoy(int amount);
	virtual void collectGold() = 0;

	void setLeaving() { m_leave = true; }
	bool isLeaving() { return m_leave; }

	void pickNewRandomDir();
	void makeRightAngleTurn();

	void setRestTicks(int ticks) { m_rest = ticks; }
	int  getRestTicks() const { return m_rest; }

	void setShoutRecovery(int amount) { m_shoutRecovery = amount; }
	int  getShoutRecovery() const { return m_shoutRecovery; }
	void setRightAngleTurnRecovery(int amount) { m_rightAngleTurnRecovery = amount; }
	int  getRightAngleTurnRecovery() const { return m_rightAngleTurnRecovery; }

	int getDistancedTraveled() const { return m_distanceInCurDir; }

	void addToPath(int x, int y, Direction d) { path.push(pathNode(x, y, d)); }

private:
	bool m_leave; // leaving state of Protester
	int m_shoutRecovery; // rest time after a shout

	int m_distanceInCurDir;
	int m_rightAngleTurnRecovery;
	int m_rest;

	struct pathNode
	{
		pathNode(int x, int y, Direction d) : m_x(x), m_y(y), m_dir(d) {}
		int m_x;
		int m_y;
		Direction m_dir;
	};

	std::queue<pathNode> path;
};

/*
[] What a Regular Protester object Must Do When It Is Created
-------------------------------------------------------------
When it is first created:
1. Each Regular Protester must have an image ID of IMID_PROTESTER.
2. Each Regular Protester must start out facing left.
3. Each Regular Protester must decide how many squares,
numSquaresToMoveInCurrentDirection, it will decide to move left before
possibly switching its direction. This value must be: 8 <=
numSquaresToMoveInCurrentDirection <= 60
4. Each Regular Protester starts out with 5 hit-points.
5. Each Regular Protester starts out NOT in a leave-the-oil-field state.
6. Each Regular Protester have the following graphic parameters:
a. It has an image depth of 0 Ò theyÌre always in the foreground
b. It has a size of 1.0
*/

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world, int ticks)
		: Protester(world, IMID_PROTESTER, 5, ticks) {}

	// overloaded c'tor for testing purposes only!!!
	RegularProtester(StudentWorld* world, int x, int y, int ticks)
		: Protester(world, x, y, IMID_PROTESTER, 5, ticks) {}

	virtual void collectGold();
private:

};

/*
[] What a Hardcore Protester object Must Do When It Is Created
--------------------------------------------------------------
When it is first created:
1. Each Hardcore Protester must have an image ID of IMID_HARD_CORE_PROTESTER.
2. Each Hardcore Protester must start out facing left.
3. Each Hardcore Protester must decide how many squares,
numSquaresToMoveInCurrentDirection, it will decide to move left before
possibly switching its direction. This value must be: 8 <=
numSquaresToMoveInCurrentDirection <= 60
4. Each Hardcore Protester starts out with 20 hit-points.
5. Each Hardcore Protester starts out NOT in a leave-the-oil-field state.
6. Each Hardcore Protester have the following graphic parameters:
a. It has an image depth of 0 Ò theyÌre always in the foreground
b. It has a size of 1.0

In addition to any other initialization that you decide to do in your Hardcore Protesters class,
a Hardcore Protester object must set itself to be visible using the GraphObject classÌs setVisible() method, e.g.:

setVisible(true);
*/

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world, int ticks)
		: Protester(world, IMID_HARD_CORE_PROTESTER, 20, ticks) {}

	virtual ~HardcoreProtester() {}
	virtual bool hasCellPhoneTracker() { return true; }
	virtual void collectGold();
private:

};



class Dirt : public Actor
{
public:
	Dirt(StudentWorld* world, int x_pos, int y_pos)
		: Actor(world, IMID_DIRT, x_pos, y_pos, right, .25, 3)
	{
		setVisible(true);
	}
	virtual ~Dirt() {}
	virtual void doSomething() { return; }
private:


};


class Boulder : public Actor
{
public:
	enum BoulderState { STABLE, WAITING, FALLING };


	Boulder(StudentWorld* world, int x_pos, int y_pos)
		: Actor(world, IMID_BOULDER, x_pos, y_pos, down, 1.0, 1), m_state(STABLE), m_stationary(true), m_wait(30)
	{
		setVisible(true);
	}
	virtual ~Boulder() {}
	virtual void doSomething();
	virtual bool canBlockCharacters() { return m_stationary; }


private:
	BoulderState m_state;
	bool  m_stationary;
	int m_wait;
};



class Squirt : public Actor
{
public:
	Squirt(StudentWorld* world, int x_pos, int y_pos, Direction dir)
		: Actor(world, IMID_WATER_SPURT, x_pos, y_pos, dir, 1.0, 1), m_travelDistance(4)
	{
		setVisible(true);


	}
	virtual ~Squirt() {}
	virtual void doSomething();
private:
	int m_travelDistance;
};


class Item : public Actor
{
public:
	Item(StudentWorld* world, int imgID, Misc itemType, int x_pos, int y_pos,
		Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(world, imgID, x_pos, y_pos, dir, size, depth), m_item(itemType) {}
	virtual ~Item() {}
	virtual void doSomething();
	Misc getItemType() { return m_item; }
	void collection();


private:
	Misc m_item;
};



class VanishingItem : public Item
{
public:
	VanishingItem(StudentWorld* world, int imgID, Misc itemType, int x_pos, int y_pos, int ticks,
		Direction dir = right, double size = 1.0,
		unsigned int depth = 0)
		: Item(world, imgID, itemType, x_pos, y_pos, dir, size, depth), m_ticks(ticks) {}
	virtual ~VanishingItem() {}
	virtual void doSomething();


	int getTicks() { return m_ticks; }
	void decrementTick() { m_ticks--; }


private:
	int m_ticks;


};


class SonarKit : public VanishingItem
{
public:
	SonarKit(StudentWorld* world, int ticks)
		: VanishingItem(world, IMID_SONAR, SONARKIT, 0, 60, ticks, right, 1.0, 2)
	{
		setVisible(true);
	}
	virtual ~SonarKit() {}


private:


};


class DroppedNugget : public VanishingItem
{
public:
	DroppedNugget(StudentWorld* world, int x_pos, int y_pos, int ticks)
		: VanishingItem(world, IMID_GOLD, DROPPEDNUGGET, x_pos, y_pos, ticks, right, 1.0, 2)
	{
		setVisible(true);
	}
	virtual ~DroppedNugget() {}
private:
};

class WaterPool : public VanishingItem
{
public:
	WaterPool(StudentWorld* world, int x_pos, int y_pos, int ticks)
		: VanishingItem(world, IMID_WATER_POOL, WATERPOOL, x_pos, y_pos, ticks, right, 1.0, 2)
	{
		setVisible(true);
	}
private:
};

class BarrelOfOil : public Item
{
public:
	BarrelOfOil(StudentWorld* world, int x_pos, int y_pos)
		: Item(world, IMID_BARREL, BARRELOFOIL, x_pos, y_pos, right, 1.0, 2)
	{
		setVisible(false);
	}
	virtual ~BarrelOfOil() {}
private:
};


class GoldNugget : public Item
{
public:
	GoldNugget(StudentWorld* world, int x_pos, int y_pos)
		: Item(world, IMID_GOLD, GOLDNUGGET, x_pos, y_pos, right, 1.0, 2)
	{
		setVisible(false);
	}
	virtual ~GoldNugget() {}
private:
};

#endif // ACTOR_H_
