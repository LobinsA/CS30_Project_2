#include "StudentWorld.h"
#include "Actor.h"
#include <string>
using namespace std;



GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}


StudentWorld::StudentWorld(std::string assetDir) : GameWorld(assetDir), m_user(nullptr)
{
    // initialize to null to prevent potential access of random garbage memory
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
            m_land[x][y] = nullptr;
    // land is initialized
}


StudentWorld::~StudentWorld()
{
    cleanUp();
}


/*
 [] refillField function info
 ---------------------------
 The oil field, which is comprised of 1x1 Dirt objects,
 occupies 64x60 squares of the screen
 Hence, the VIEW_HEIGHT - SPRITE_HEIGHT in the 2nd for loop,
 where VIEW_HEIGHT is 64 and SPRITE_HEIGHT is 4... 64-4 = 60!
 */
void StudentWorld::refillField()
{
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT - SPRITE_HEIGHT; y++)
            m_land[x][y] = new Dirt(this, x, y);
}


/*
 [] buildMineShaft function info
 ------------------------------
 A single mineshaft is:
 4 squares wide (occupying columns 30-33 of the oil field),
 and 56 squares deep (occupying rows 4-59)
 and must be devoid of any Dirt objects.
 */
void StudentWorld::buildMineShaft()
{
    for (int x = 30; x < 34; x++)
        for (int y = 4; y < 60; y++)
        {
            delete m_land[x][y];
            m_land[x][y] = nullptr;
        }
}


void StudentWorld::placeObjects()
{
    
    
    // place Boulder object as test
    Actor* new_boulder = new Boulder(this, 60, 54);
    removeDirt(new_boulder);
    insertActor(new_boulder);
    
    
    Actor* new_boulder2 = new Boulder(this, 60, 30);
    removeDirt(new_boulder2);
    insertActor(new_boulder2);
    
    
    // place barrel of oil test
    Actor* new_oilbarrel = new BarrelOfOil(this, 5, 54);
    insertActor(new_oilbarrel);
    
    
    // get numof ticks from current level
    int sonarkitTicks = max(100, (300 - 10 * static_cast<int>(getLevel()))); // NEW!!!
    
    
    // place sonarkit as test
    Actor* new_sonarkit = new SonarKit(this, 10, 55, sonarkitTicks);
    insertActor(new_sonarkit);
    
    
    Actor* new_goldnugg = new GoldNugget(this, 40, 55);
    insertActor(new_goldnugg);
    
    
    /*
     [] ticksToWaitBetweenMoves info
     -------------------------------
     The Regular and Hardcore Protesters must compute a value
     indicating how often they're allowed to take an action (e.g., once every N ticks).
     This number of ticks (also known as 'resting ticks') may be computed as follows:
     
     int ticksToWaitBetweenMoves = max(0, 3 - current_level_number/4)
     */
    
    int ticksToWaitBetweenMoves = max(0, 3 - static_cast<int>(getLevel() / 4));
     Actor* new_RegProtestor = new RegularProtester(this, ticksToWaitBetweenMoves);
    insertActor(new_RegProtestor);
    
    Actor* new_HardcoreProtestor = new HardcoreProtester(this, ticksToWaitBetweenMoves);
    insertActor(new_HardcoreProtestor);
    
    
    
    
    // B Boulders in each level, where:
    // int B = min(static_cast<int>(getLevel()) / 2 + 2, 7);
    
    
    // G Gold Nuggets in each level, where:
    // int G = max(5-static_cast<int>(getLevel()) / 2, 2);
    
    
    // L Barrels of oil in each level, where:
    // int L = min(2 + static_cast<int>(getLevel()), 18);
}


void StudentWorld::insertActor(Actor* entry)
{
    m_actors.push_back(entry);
}


/*
 [] Note about the handling of actors within the world
 -----------------------------------------------------
 All of the actors (including the DiggerMan)
 will be inserted into the m_actors vector by insertActor function
 So responsiblities such as deletion will be handled by the vector
 side note: m_actors[0] is always reserved for the DiggerMan (user-controlled player)
 */
int StudentWorld::init()
{
    // pass 'this' so Diggerman object has access/reference to contents of this StudentWorld
    m_user = new DiggerMan(this);
    
    
    // insert the actor into the vector
    insertActor(m_user);
    
    
    // create dirt objects
    refillField();
    
    
    // build mine shaft
    buildMineShaft();
    
    
    // distribute game objects around field
    placeObjects();
    
    
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    // check if player is alive
    if (m_user == nullptr)
    {
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
        return GWSTATUS_PLAYER_DIED;
    }
    
    
    // asks all the actors to do something
    size_t size = m_actors.size();
    for (size_t i = 0; i < size; i++)
    {
        if (m_actors[i]->isAlive())
        {
            m_actors[i]->doSomething();
        }
    }
    
    
    // remove any dead actors
    removeDead();
    // continue to next tick
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    // delete all actors (which includes the DiggerMan)
    for (size_t i = 0; i < m_actors.size(); i++)
        delete m_actors[i];
    m_actors.clear();
    
    m_user = nullptr; // prevent dangling ptr
    
    // delete the dirt objects
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
            delete m_land[x][y];
}

/*
 [] updateCoord function info
 --------------------------------
 This function will take in the:
 - current position of the actor
 - along with it's direction
 The direction it is facing will determine whether it goes:
 up, down, left or right
 The distance argument is initialized to a default of 1
 in the function declaration in StudentWorld.h
 Characters can only move a distance of 1 square/block at a time (or tick)
 However, the distance argument can take an argument other than 1
 */
void StudentWorld::updateCoord(GraphObject::Direction dir, int& x, int& y, int distance)
{
    // works like the cartesian coordinate system in math
    if (dir == GraphObject::up)
        y += distance;
    else if (dir == GraphObject::down)
        y -= distance;
    else if (dir == GraphObject::right)
        x += distance;
    else if (dir == GraphObject::left)
        x -= distance;
}


/*
 [] outOfBounds function info
 --------------------------------
 Recall that the game screen is 64x64
 The sprites (aka images) of the actor objects is 4x4
 Any x coordinate more than 60, or less than 0
 Any y coordinate more than 60, or less than 0
 will cause the sprite to be rendered outside of the 64x64 game screen,
 which will cause undefined behavior or a program crash
 */
bool StudentWorld::outOfBounds(const int& x, const int& y) const
{
    if ((x < 0 || x >VIEW_WIDTH - SPRITE_HEIGHT) || (y < 0 || y > VIEW_HEIGHT - SPRITE_HEIGHT))
        return true;
    else
        return false;
}


/*
 [] BlockedByBoulder function info
 -------------------------------------
 This function gets passed in an:
 - actor object along with it's desired (x, y) coordinate
 If the desired (x, y) coordinate
 is within the radius of a Boulder (radius of 3.0),
 then the function will return true,
 which will prevent the move to that desired (x, y) coordinate
 
 
 We'll have to iterate through the actors vector to look for Boulders
 canBlockCharacters() signifies that the actor in the vector is a Boulder
 It's a trait that differentiates it from all the other actors in the vector
 Just like how canCollectItems() signifies that its the DiggerMan or a Protestor
 
 
 The if statement:
 if (m_actors[i] == OneSelf) {continue;}
 prevents the actor object to be compared with itself in the vector
 We'll have errors in our game if we do so
 */
bool StudentWorld::BlockedByBoulder(Actor* OneSelf, int x, int y) const
{
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        // prevent comparison of actor to itself as we iterate thru vector, skip it!
        if (m_actors[i] == OneSelf)
            continue;
        if (m_actors[i]->canBlockCharacters() && withinRad(x, y, m_actors[i], 3))
            return true;
    }
    return false;
}


/*
 [][][][]
 [][][][]
 [][][][]
 [][][][]
 */
bool StudentWorld::LayerOfDirt4x4(int x, int y) const
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (m_land[x + i][y + j] != nullptr)
                return true;
    
    
    return false;
}


/*
 [][][][]
 */
bool StudentWorld::LayerOfDirt4x1(int x, int y) const
{
    for (int i = 0; i < 4; i++)
        if (m_land[x + i][y] != nullptr)
            return true;
    return false;
}


/*
 [] checkItemPickup function info
 --------------------------------
 If collectable item is within radius of 3.0 of user,
 then item will be acquired by user
 
 o Barrel of oil: 1000 points
    the DiggerMan must collect all of the Barrels
    on the oil field to complete the level
 o Gold Nugget: 10 points
    the DiggerMan gets 1 piece of gold added to their inventory,
    which they can subsequently drop to bribe Protesters
 o Water: 100 points
    5 squirts worth of water are added to the DiggerMan's inventory
 o Sonar Kit: 75 points
    the DiggerMan gets two new sonar charges added to their inventory
 
 [] Special Case (items non-collectable by user)
 --------------------------------------------
 If item is a dropped gold nugget,
 iterate through m_actors vector to find Protesters
 If a Protester is found,
 pass Protester into checkdroppedNugget function
 along with the dropped gold nugget object
 Then, checkdroppedNugget will check if Protester
 is within radius of 3.0 of the dropped gold nugget
 
 Different results will occur if its a Regular or Hardcore Protester
 
 (i.e.)
 A Regular Protester gets bribed
 while a Hardcore Protester looks at gold, then keeps it
 */

void StudentWorld::checkItemPickup(Item* obj) // UNDER CONSTRUCTION  NEW!!!! (edited)
{
    if (obj->getItemType() == Actor::DROPPEDNUGGET) {
        // iterate thru vector and find any Protesters
        for (size_t i = 0; i < m_actors.size(); i++) {
            if (m_actors[i]->canCollectItems()) {
                if (Protester* p = dynamic_cast<Protester*>(m_actors[i]))
                    checkdroppedNugget(obj, p);
            }
        }
    }
    
    else {
        if (withinRad(obj, m_user, 3)) {
            Actor::Misc item = obj->getItemType();
            switch (item)
            {
                case Actor::BARRELOFOIL:
                    // ** decrement oil count from world **
                    playSound(SOUND_FOUND_OIL);
                    increaseScore(1000);
                    break;
                case Actor::SONARKIT:
                    m_user->gainSonarCharge();
                    playSound(SOUND_GOT_GOODIE);
                    increaseScore(75);
                    break;
                case Actor::GOLDNUGGET:
                    m_user->gainGoldNugget();
                    playSound(SOUND_GOT_GOODIE);
                    increaseScore(10);
                    break;
                    // case Actor::WATERPOOL:
                    // m_user->refillSquirtGun()
                    // play the sound of water getting collected?
                    // increaseScore(100);
            }
            obj->setDead();
        }
    }
}


void StudentWorld::checkdroppedNugget(Item* obj, Protester* CPU) // NEW!!!! (edited)
{
    if (CPU->isLeaving())
        return;
    
    if (withinRad(obj, CPU, 3)) {
        CPU->collectGold();
        playSound(SOUND_PROTESTER_FOUND_GOLD);
        obj->setDead();
    }
}


bool StudentWorld::proximityCheck(Actor* actor, int distance)
{
    if (withinRad(actor, m_user, distance))
        return true;
    else
        return false;
}


void StudentWorld::shootSquirtGun() // NEW!!!!! (edited)
{
    // get user's current coordinate
    int x = m_user->getX();
    int y = m_user->getY();
    
    // update coordinate after squirt object's potential move
    updateCoord(m_user->getDirection(), x, y, SPRITE_WIDTH /*aka 4*/);
    
    bool blocked = m_user->Actor::coordinateCheck(x, y);
    
    // if a spawned squirt object will go out-of-bounds or make contact with Boulder or dirt, return
    if (blocked)
        return;
    else // Otherwise, spawn a squirt object into field with user's current direction
        insertActor(new Squirt(this, x, y, m_user->getDirection()));
}


/*
 [] dropGoldNugget function info
 ---------------------------------
 A dropped Gold Nugget has a temporary state and a lifetime of 100 game ticks
 */
void StudentWorld::dropGoldNugget()
{
    insertActor(new DroppedNugget(this, m_user->getX(), m_user->getY(), 100));
}


/*
 [] withinRad functions info
 --------------------------
 There's 2 versions of the withinRad function (short for "within radius")
 Its an overloaded function
 
 
 The difference between the 2 versions is:
 bool withinRad(double x1, double y1, Actor* P2, double radius)
 This verison tests for a potential move to an (x, y) coodinate
 The actor has not yet moved to that coordinate,
 we're testing whether or not it's able to move there
 This version is in use in bool Actor::makeMove -> bool StudentWorld::BlockedByBoulder
 where a stationary Boulder would block an actor's move
 
 
 Wheras:
 bool StudentWorld::withinRad(Actor* P1, Actor* P2, double radius)
 This version takes already existing Actor objects,
 and tests whether or not they're within a certain radius of each other
 This version is used for:
 - falling Boulders crushing any Character(s) that are nearby
 - squirt objects making contact with a Dirt object, Protesters, Boulder, etc.
 - Whether a Character is within a certain radius of an item,
 so the item is then made visible
 - item pick-ups by a Character
 etc. etc.
 
 
 The 2 versions serve the same purpose,
 but used under different circumstances
 */


bool StudentWorld::withinRad(double x1, double y1, Actor* P2, double radius) const
{
    // r = sqrt[(x2-x1)^(2) + (y2-y1)^(2)]
    double x = P2->getX() - x1;
    double y = P2->getY() - y1;
    
    
    double P1toP2 = sqrt((x*x) + (y*y));
    
    
    if (P1toP2 <= radius)
        return true;
    else
        return false;
}


bool StudentWorld::withinRad(Actor* P1, Actor* P2, double radius) const
{
    // r = sqrt[(x2-x1)^(2) + (y2-y1)^(2)]
    double x = P2->getX() - P1->getX();
    double y = P2->getY() - P1->getY();
    
    
    double P1toP2 = sqrt((x*x) + (y*y));
    
    
    if (P1toP2 <= radius)
        return true;
    else
        return false;
}


/*
 [] CPUspotsUser function info
 -----------------------------
 Calculate change in x and change in y from CPU to the user
 
 (positive) change in y: user is above CPU
 (negative) change in y: user is below CPU
 
 (positive) change in x: user is right of CPU
 (negative) change in x: user is left of CPU
 
 [] bool flags [within_x_fov and within_y_fov]
 -----------------------------------
 These check if user is within field of view of CPU
 
 <-------------->
 y field of view (used when user is left or right of CPU)
 <-------------->
 
 ^                 ^
 | x field of view | (used when user is above or below CPU)
 v                 v
 */

bool StudentWorld::CPUspotsUser(Actor* CPU)
{
    if (withinRad(CPU, m_user, 4)) {
    
        bool within_x_fov = (m_user->getX() >= CPU->getX() && m_user->getX() <= CPU->getX() + 3);
        bool within_y_fov = (m_user->getY() >= CPU->getY() && m_user->getY() <= CPU->getY() + 3);
        
        int deltaX = m_user->getX() - CPU->getX();
        int deltaY = m_user->getY() - CPU->getY();
        
        GraphObject::Direction curDir = CPU->getDirection();
        
        if (curDir == GraphObject::up)
            return deltaY > 0 && within_x_fov;
        if (curDir == GraphObject::down)
            return deltaY < 0 && within_x_fov;
        if (curDir == GraphObject::right)
            return deltaX > 0 && within_y_fov;
        if (curDir == GraphObject::left)
            return deltaX < 0 && within_y_fov;
    }
    return false;
}

/* 
 [] CPUspotsUserfromAFar info
 ----------------------------
 First, the function checks if CPU
 isn't within a radius of 4 of the user
 If it passes that requirement,
 this function will then check if CPU is in:
    same level vertically or horizontally of the user
 
 Not only that, but also, if there's no boulder(s) or dirt blocking the path
 The CPU can't see through obstacles (such as dirt or boulders)
 The current direction of the CPU doesn't matter at all
 */
bool StudentWorld::CPUspotsUserfromAFar(Actor* CPU)
{
    if (!withinRad(CPU, m_user, 4)) {
        
        // get CPU's current coordinate
        int x = CPU->getX();
        int y = CPU->getY();
    
        GraphObject::Direction dirToUser;
        
        // CASE 1: DIRECT LINE-OF-SIGHT VERTICALLY
        if (m_user->getX() == CPU->getX()) {
            
            // calculate change in y
            int deltaY = m_user->getY() - CPU->getY();
            
            if (deltaY > 0) // if user is above CPU
                dirToUser = GraphObject::up;
            else // if user is below CPU
                dirToUser = GraphObject::down;
            
            // check whether boulders or dirt are blocking line-of-sight to user
            for (int i = 0; i < abs(deltaY); i++) {
                updateCoord(dirToUser, x, y);
                if (CPU->coordinateCheck(x, y))
                    return false;
            }
            // if not, set CPU to face the user
            CPU->setDirection(dirToUser);
            return true;
        }
        
        // CASE 2: DIRECT LINE-OF-SIGHT HORIZONTALLY
        else if (m_user->getY() == CPU->getY()) {
            
            // calculate change in x
            int deltaX = m_user->getX() - CPU->getX();
            
            if (deltaX > 0) // if user is right of CPU
                dirToUser = GraphObject::right;
            else // if user is left of CPU
                dirToUser = GraphObject::left;
            
            // check whether boulders or dirt are blocking line-of-sight to user
            for (int i = 0; i < abs(deltaX); i++) {
                updateCoord(dirToUser, x, y);
                if (CPU->coordinateCheck(x, y))
                    return false;
            }
            // if not, set CPU to face the user
            CPU->setDirection(dirToUser);
            return true;
        }
    }
    return false;
}


void StudentWorld::shout(int amount)
{
    playSound(SOUND_PROTESTER_YELL);
    m_user->annoy(amount);
}


bool StudentWorld::annoyCharactersNearby(Actor* annoyer, int radius, int points, Actor::Misc object) // NEW!!!! (edited)
{
    size_t i;
    bool annoyed = false;
    
    if (object == Actor::SQUIRT)
        i = 1; // skip user, don't want user to shoot itself
    else
        i = 0;
    
    while (i < m_actors.size()) {
        if (m_actors[i]->canCollectItems()
            && withinRad(annoyer, m_actors[i], 3)) {
            m_actors[i]->annoy(points);
            annoyed = true;
        }
        i++;
    }
    return annoyed;
}



void StudentWorld::scanForItems()
{
    // For some reason, we can't use isVisible()
    // Find an alternative, or just remove isVisible all together
    
    // go through actors
    for (size_t i = 1; i < m_actors.size(); i++) {
        // check if object isn't visible && within radius
        if (!m_actors[i]->isVisible() && withinRad(m_user, m_actors[i], 12))
            m_actors[i]->setVisible(true);
    }
}

/*
 [] removeDirt function info
 --------------------------
 The Characters and Boulders have a 4x4 image
 that overlaps any Dirt objects within the oil field
 
 
 Recall that a coordinate (x, y)
 represents the lower-leftmost position of the spite
 [ ][ ][ ][ ]
 [ ][ ][ ][ ]
 [ ][ ][ ][ ]
 [*][ ][ ][ ]
 The sprite (aka image) covers 4x4 squares,
 any dirt within the 4x4 image is deleted
 */

bool StudentWorld::removeDirt(Actor* actor)
{
    bool removed = false;
    for (int x = actor->getX(); x < actor->getX() + SPRITE_WIDTH; x++)
    {
        for (int y = actor->getY(); y < actor->getY() + SPRITE_HEIGHT; y++)
        {
            if (m_land[x][y] != nullptr)
            {
                delete m_land[x][y];
                m_land[x][y] = nullptr;
                removed = true;
            }
        }
    }
    return removed;
}

/*
 [] std::vector::erase info [source: cppreference]
 -------------------------
 erase(iterator pos);
 -- Parameters --
 pos: iterator to the element to remove
 -- Return value --
 Iterator following the last removed element.
 If the iterator pos refers to the last element,
 the end() iterator is returned.
 */
void StudentWorld::removeDead()
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end();)
    {
        if ((*i)->isAlive() == false) {
            delete *i;
            i = m_actors.erase(i); // will return iterator following the removed element
        }
        else {
            ++i; // increment only when necessary
        }
    }
    if (m_actors[0] != m_user)
        m_user = nullptr;
}

///////////////////////////////
//// NEW FUNCTIONS ADDED /////
/////////////////////////////

bool StudentWorld::atAnIntersection(Actor* CPU)
{
    // get user's current coordinate
    int x = CPU->getX();
    int y = CPU->getY();
    
    // check if one square [up, down, left, or right] from CPU is blocked
    for (int i = -1; i < 2; i += 2)
        if (CPU->coordinateCheck(x, y+i) || CPU->coordinateCheck(x+i, y))
            return false;
    // if not, we're at an intersection
    return true;
}

/*
 David Note
 ----------
 The following are temporary solutions to access the function max (from <algorithm>)
 in the Actor files, we get an error otherwise for some reason :\
 This will do for now, but if a solution is found where we can
 access function max in both StudentWorld files and Actor files, then that'll be good
 
 Or maybe function(s) where we can gather all 
 the formulas given in the project specs is good?
 So we don't have random usage of formulas scattered throughout the code?
 Idk, you'll be the judge
 */

/*
 [] ProtesterRestTicks info
 --------------------------
 A Protester must compute a value
 indicating how often they’re allowed to take an action (e.g., once every N ticks).
 This number of ticks (also known as “resting ticks”) may be computed as follows:
 
	int ticksToWaitBetweenMoves = max(0, 3 – current_level_number/4)

 */
int StudentWorld::ProtesterRestTicks()
{
    int ticksToWaitBetweenMoves = max(0, 3 - static_cast<int>(getLevel() / 4));
    return ticksToWaitBetweenMoves;
}

/*
 [] ProtesterStunTicks info
 --------------------------
 After its hit-points have been decremented, and
 the Protester hasn’t been completely annoyed
 (to the point it wants to leave the oil field)
 It will then be “stunned” and placed in a
 resting state for N resting ticks, where:
 
	N = max(50, 100 – current_level_number * 10)
 
 After picking up gold nugget,
 the Hardcore Protester will become fixated on the Nugget and
 will pause to stare at it (just as if he/she were in a resting state – doing nothing else) for the following number of game ticks:
	ticks_to_stare = max(50, 100 – current_level_number * 10)

 
 */
int StudentWorld::ProtesterStunTicks()
{
    int N = max(50, 100 - static_cast<int>(getLevel() * 10));
    return N;
}

// Students: Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
