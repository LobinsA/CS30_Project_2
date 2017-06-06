#include "Actor.h"
#include "StudentWorld.h"


// Students: Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


/*
 [] makeMove function info
 ------------------------
 A move operation for Actors that takes into account
 what blocks/stops an individual Actor's move
 
 (for ex.)
 What blocks Boulders, Squirts, or Protesters?:
 - out-of-bounds, dirt, boulders
 What blocks DiggerMan?:
 - out-of-bounds, boulders
 
 The coordinateCheck function will handle these differences
 */
bool Actor::makeMove()
{
    // get actor's current coordinate (x, y)
    int x = getX();
    int y = getY();
    
    // update coordinate after actor's potential move
    accessToWorld()->updateCoord(getDirection(), x, y);
    
    // Check if updated (x, y) coordinate will be blocked
    // Depending on which Actor it is,
    // there will be different criterias
    bool moveBlocked = coordinateCheck(x, y);
    
    if (moveBlocked)
        return false;
    else
        moveTo(x, y);
    return true;
}


void DiggerMan::doSomething()
{
    // check alive status
    if (isAlive() == false)
        return;
    
    // get user input
    int userInput;
    if (accessToWorld()->getKey(userInput))
    {
        switch (userInput)
        {
            case KEY_PRESS_UP:
                if (getDirection() != up)
                    setDirection(up);
                else if (getDirection() == up)
                    makeMove();
                break;
                
                
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                    setDirection(down);
                else if (getDirection() == down)
                    makeMove();
                break;
                
                
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                    setDirection(right);
                else if (getDirection() == right)
                    makeMove();
                break;
                
                
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                    setDirection(left);
                else if (getDirection() == left)
                    makeMove();
                break;
                
                
            case 'z':
            case 'Z':
                if (getSonarCharges() > 0)
                {
                    accessToWorld()->playSound(SOUND_SONAR);
                    m_sonarcharges--;
                    accessToWorld()->scanForItems();
                }
                break;
            case KEY_PRESS_SPACE:
                if (getSquirts() > 0)
                {
                    accessToWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    accessToWorld()->shootSquirtGun();
                    m_squirts--;
                }
                break;
                
                
            case KEY_PRESS_TAB:
                if (getGoldNuggets() > 0)
                {
                    accessToWorld()->dropGoldNugget();
                    m_goldnuggets--;
                }
                break;
                // custom spawn where the DiggerMan is (for testing purposes only)!!!
            case 'c':
            case'C':
                int T = accessToWorld()->ProtesterRestTicks();
                accessToWorld()->insertActor(new RegularProtester(accessToWorld(), getX(), getY(), T));
                accessToWorld()->incrProtesterCount();
                break;
        }
        // see if 4x4 sprite overlaps any dirt
        bool dirtWasRemoved = accessToWorld()->removeDirt(this);
        
        
        if (dirtWasRemoved)
            accessToWorld()->playSound(SOUND_DIG);
    }
}




void DiggerMan::annoy(int amount)
{
    subtractFromHP(amount);
    if (getTotalHP() <= 0)
        setDead();
}


/*
 [] Protesters, unlike the player,
 don't necessarily get to take an action during every tick of the game
 
 [] The Protester must therefore compute a value indicating
 how often they're allowed to take an action (e.g., once every N ticks).
 This number of ticks (also known as 'resting ticks') may be computed as follows:
 
 int ticksToWaitBetweenMoves = max(0, 3 - current_level_number/4)
 
 So if the value of ticksToWaitBetweenMoves was 3,
 then the Regular Protester must 'rest' for 3 ticks and
 may perform its normal behavior every 4th tick
 */

void Protester::doSomething() // UNDER CONSTRUCTION
{
    // (1) if Protester is alive
    if (!isAlive())
        return;
    
    
    // (2) if Protester is in "rest state"
    if (getRestTicks() > 0) {
        --m_rest; // if so, decrement a tick
        return;
    }
    
    setRestTicks(accessToWorld()->ProtesterRestTicks());
    m_shoutRecovery--;
    m_rightAngleTurnRecovery--;
    
    // (3) complicated queue-based maze searching portion (descript. found on pg. 40)
    if (isLeaving()) {
        // check if protester has a path to the exit
        if (!path.empty()) {
            // if so, move one coordinate at a time towards the exit
            pathNode p = path.front();
            setDirection(p.m_dir);
            moveTo(p.m_x, p.m_y);
            path.pop();
            // the last remaining element in the queue should be the exit coordinate
            if (getX() == 60 && getY() == 60) {
                setDead();
                accessToWorld()->decProtesterCount();
            }
        }
        else {
            accessToWorld()->BFS(node(60, 60), this);
            accessToWorld()->followShortestPath(this, node(60, 60));
            // reset the maze for future use (i.e. when another Protester is set to leaving state)
            accessToWorld()->resetNodeMaze();
        }
        return;
    }
    
    // (4) if Protester is within 4 units of user AND is facing user
    if (accessToWorld()->CPUspotsUser(this)) {
        
        if (getShoutRecovery() <= 0) {
            accessToWorld()->shout(2);
            setShoutRecovery(15);
            return;
        }
    }
    
    
    // (4.5) [specific Hardcore Protester action] (descript found on pg. 45)
    if (hasCellPhoneTracker() && !accessToWorld()->proximityCheck(this, 4))
    {
        if (!path.empty()) {
            pathNode p = path.front();
            setDirection(p.m_dir);
            moveTo(p.m_x, p.m_y);
            path.pop();
            setRestTicks(accessToWorld()->ProtesterRestTicks());
            return;
        }
        else {
            doSomething_aux();
            if (!path.empty())
                return;
        }
    }
    
    
    // (5) if user not within 4 units of user
    // but is in Protester's unblocked line of sight
    if (accessToWorld()->CPUspotsUserfromAFar(this)) {
        makeMove();
        m_distanceInCurDir = 0;
        return;
    }
    
    m_distanceInCurDir--;
    
    // (6) If protester has finished traveling, pick a new direction
    if (getDistancedTraveled() <= 0) {
        pickNewRandomDir();
        m_distanceInCurDir = rand() % 53 + 8; // [0-52] + 8 ==> [8-60]
    }
    
    // (7) While traveling, check if Protester is at an intersection
    else if (getDistancedTraveled() > 0)
    {
        if (accessToWorld()->atAnIntersection(this) && getRightAngleTurnRecovery() <= 0) {
            makeRightAngleTurn();
            setRightAngleTurnRecovery(200);
            m_distanceInCurDir = rand() % 53 + 8;
        }
    }
    
    // (8) & (9) Attempt to take step in current direction
    // If it is somehow blocked, set distance to 0, then rinse & repeat this whole process
    if (!makeMove())
        m_distanceInCurDir = 0;
}

void HardcoreProtester::doSomething_aux()
{
    // a. Compute a value M, such that M = 16 + current_level_number * 2
    int M = 16 + accessToWorld()->getLevel() * 2;
    /*
     b.If the Hardcore Protester is less than or equal to
     a total of M legal horizontal or vertical moves away from the current location of the DiggerMan
     (as can be determined using the same basic queue - based maze - searching algorithm)
     */
    int stepsToUser = accessToWorld()->BFS(node(user_tracker->getX(), user_tracker->getY()), this);
    
    if (stepsToUser <= M) {
        accessToWorld()->playSound(SOUND_FALLING_ROCK);
        accessToWorld()->followShortestPath(this, node(user_tracker->getX(), user_tracker->getY()));
        
    }
    accessToWorld()->resetNodeMaze();
}

/*
 [] Protester::annoy function info
 ---------------------------------
 For causing a Boulder to bonk a Protester:    500 points
 For causing a Hardcore Protester to give up:  250 points
 For causing a Regular Protester to give up:   100 points
 
 The following is the same for Regular & Hardcore Protesters:
 If, after its hit-points have been decremented,
 the Protester hasnÌt been completely annoyed
 
 It will then be 'stunned' and placed in a resting state for N resting ticks, where:
 
 N = max(50, 100 - current_level_number * 10)
 
 Recall that falling boulders causes 100 points of annoyance
 */

void Protester::annoy(int amount) // UNDER CONSTRUCTION // NEW!!! (edited)
{
    if (isLeaving())
        return;
    
    subtractFromHP(amount);
    
    if (getTotalHP() <= 0) {
        accessToWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        setLeaving();
        
        if (amount == 100) // bonked by boulder
            accessToWorld()->increaseScore(500);
        else if (hasCellPhoneTracker()) // made Hardcore Protester give up (via squirt gun)
            accessToWorld()->increaseScore(250);
        else if (!hasCellPhoneTracker())// made Regular Protester give up (via squirt gun)
            accessToWorld()->increaseScore(100);
    }
    
    else if (getTotalHP() > 0) {
        accessToWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        int stunnedTicks = accessToWorld()->ProtesterStunTicks();
        setRestTicks(stunnedTicks);
    }
}


/*
 [] RegularProtester::collectGold info (pg. 43)
 -------------------------------------
 When a Gold Nugget is collected by a Regular Protester
 The Regular Protester must do the following in response:
 1. The Regular Protester plays an 'I'm rich' sound effect:
 SOUND_PROTESTER_FOUND_GOLD.
 2. The Regular Protester increases the player's score by 25 points for the bribery.
 3. The Regular Protester will immediately be bribed and transition into a leave-the-oil-field state.
 */

void RegularProtester::collectGold() // UNDER CONSTRUCTION // NEW!!!! (EDITED)
{
    accessToWorld()->increaseScore(25);
    setLeaving();
}



void Boulder::doSomething() // NEW!!!! (edited)
{
    switch (m_state)
    {
        case STABLE: // check dirt beneath boulder
            if (!accessToWorld()->LayerOfDirt4x1(getX(), getY() - 1))
                m_state = WAITING;
            break;
        case WAITING: // check if wait time expires
            if (m_wait > 0)
                m_wait--;
            else if (m_wait <= 0) {
                accessToWorld()->updateNodeMaze(this);
                m_state = FALLING;
                m_stationary = false;
                accessToWorld()->playSound(SOUND_FALLING_ROCK);
            }
            break;
        case FALLING: // crush any Characters nearby
            accessToWorld()->annoyCharactersNearby(this, 3, 100, BOULDER);
            // Boulder stops if its about to go out-of-bounds
            // or hits a boulder or dirt
            if (!makeMove())
                setDead();
            break;
    }
}


void Item::doSomething()
{
    if (!isAlive())
        return;
    
    
    if (accessToWorld()->proximityCheck(this, 4))
        setVisible(true);
    
    collection();
}


void Item::collection()
{
    accessToWorld()->checkItemPickup(this);
}


void VanishingItem::doSomething()
{
    if (!isAlive())
        return;
    
    collection();
    if (getTicks() <= 0)
        setDead();
    else
        decrementTick();
}


/*
 Squirt::doSomething() function info
 -----------------------------------
 Check if any Protester(s) is within a radius
 of 3.0 of Squirt object
 
 Squirt causes 2pts of annoyance
 if it makes contact with a Protester
 
 Set Squirt to dead if it either:
 - Travels its full distance of 5 squares
 - If it makes contact with a Protester, Boulder, or dirt
 - Or if its about to go out-of-bounds
 */
void Squirt::doSomething() // NEW!!!! (edited)
{
    if (accessToWorld()->annoyCharactersNearby(this, 3, 2, SQUIRT) || (m_travelDistance <= 0))
        setDead();
    else if (!makeMove())
        setDead();
    else
        m_travelDistance--;
}


///////////////////////////////
//// NEW FUNCTIONS ADDED /////
/////////////////////////////

/*
 -- David note for pickNewRandomDir --
 There's a large tendency for the Protester to travel left or right
 when its at the very top of the field
 But its maybe for the fact that 'direction down' is a 1/4 probability,
 and the dirt beneath the Protester has to be empty as well
 for the Protester to change it's direction
 */

void Protester::pickNewRandomDir() // works fine (edited)
{
    GraphObject::Direction newDir;
    int randDir = rand() % 4;
    
    // pick new random direction to move in
    switch (randDir)
    {
        case 0:
            newDir = GraphObject::up;
            break;
        case 1:
            newDir = GraphObject::down;
            break;
        case 2:
            newDir = GraphObject::left;
            break;
        case 3:
            newDir = GraphObject::right;
            break;
    }
    
    // get CPU's current coordinate
    int x = getX();
    int y = getY();
    
    accessToWorld()->updateCoord(newDir, x, y);
    
    bool blocked = coordinateCheck(x, y);
    
    if (blocked) /* Recursive case */
        pickNewRandomDir(); // try again
    else /* base case */
        setDirection(newDir); // set new random direction
}

void Protester::makeRightAngleTurn()
{
    /* GOAL: pick a random perpendicular direction to turn */
    
    // get user's current direction
    GraphObject::Direction currentDir = getDirection();
    
    int randDir = rand() % 2;
    
    if (currentDir == GraphObject::up || currentDir == GraphObject::down) {
        switch (randDir)
        {
            case 0:
                setDirection(left);
                break;
            case 1:
                setDirection(right);
                break;
        }
    }
    
    else if (currentDir == GraphObject::left || currentDir == GraphObject::right) {
        switch (randDir)
        {
            case 0:
                setDirection(up);
                break;
            case 1:
                setDirection(down);
                break;
        }
    }
}

/*
 [] HardcoreProtester::collectGold info
 ---------------------------------
 When a Gold Nugget has activated due to being stepped upon by a Hardcore Protester
 
 Hardcore Protester must do the following:
 1. The Hardcore Protester plays an "I'm rich!" sound effect:
 SOUND_PROTESTER_FOUND_GOLD.
 2. The Hardcore Protester increases the player's score by 50 points for the bribery.
 3. The Hardcore Protester will become fixated on the Nugget and
 will pause to stare at it (just as if he/she were in a resting state doing nothing else)
 for the following number of game ticks:
 ticks_to_stare = max(50, 100 Ò current_level_number * 10)
 */
void HardcoreProtester::collectGold()
{
    accessToWorld()->increaseScore(50);
    int ticks_to_stare = accessToWorld()->ProtesterStunTicks();
    setRestTicks(ticks_to_stare);
}

bool Actor::coordinateCheck(int x, int y)
{
    // if coordinate goes out of bounds OR hits dirt OR blocked by a boulder
    if (accessToWorld()->outOfBounds(x, y) || accessToWorld()->LayerOfDirt4x4(x, y)
        || accessToWorld()->BlockedByBoulder(this, x, y))
        return true;
    else
        return false;
}

bool DiggerMan::coordinateCheck(int x, int y)
{
    // if coordinate goes out of bounds OR blocked by a boulder
    if (accessToWorld()->outOfBounds(x, y) || accessToWorld()->BlockedByBoulder(this, x, y))
        return true;
    else
        return false;
}
