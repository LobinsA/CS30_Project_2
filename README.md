# CS30_Project_2

This version fixed a crashing problem that was happening with the Boulders hitting Protesters.
Now, it is fixed

Also, ran some tests.

♥ = works as intended
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
([]) Verifying Tests for Regular Game items/objects

If collectable item is within radius of 3.0 of user,
then item will be acquired by user

♥ o Barrel of oil: 1000 points
the DiggerMan must collect all of the Barrels
on the oil field to complete the level
♥ o Gold Nugget: 10 points
the DiggerMan gets 1 piece of gold added to their inventory,
which they can subsequently drop to bribe Protesters
♥ o Water: 100 points
5 squirts worth of water are added to the DiggerMan's inventory
♥ o Sonar Kit: 75 points
the DiggerMan gets two new sonar charges added to their inventory
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
([]) Verifying Boulder points

[] Protester::annoy function info
---------------------------------
♥ For causing a Boulder to bonk a Protester:    500 points
♥ For causing a Hardcore Protester to give up:  250 points
♥ For causing a Regular Protester to give up:   100 points
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
([]) Verifying Protesters' Gold Pickup Actions

[] RegularProtester::collectGold info (pg. 43)
-------------------------------------
When a Gold Nugget is collected by a Regular Protester
The Regular Protester must do the following in response: 
♥ 1. The Regular Protester plays an 'I'm rich' sound effect:
SOUND_PROTESTER_FOUND_GOLD.
♥ 2. The Regular Protester increases the player's score by 25 points for the bribery.
♥ 3. The Regular Protester will immediately be bribed and transition into a leave-the-oil-field state. 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
[] HardcoreProtester::collectGold info
---------------------------------
When a Gold Nugget has activated due to being stepped upon by a Hardcore Protester

Hardcore Protester must do the following:
♥ 1. The Hardcore Protester plays an "I'm rich!" sound effect:
SOUND_PROTESTER_FOUND_GOLD.
♥ 2. The Hardcore Protester increases the player's score by 50 points for the bribery.
♥ 3. The Hardcore Protester will become fixated on the Nugget and 
will pause to stare at it (just as if he/she were in a resting state doing nothing else)
for the following number of game ticks:
ticks_to_stare = max(50, 100 Ò current_level_number * 10)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

([]) Verifying Squirts

♥ Each Regular Protester starts out with 5 hit-points.
♥ Each Hardcore Protester starts out with 20 hit-points

Squirt::doSomething() function info
-----------------------------------
♥ 1. Squirt causes 2pts of annoyance
♥ 2. if it makes contact with a Protester,
	set Squirt to dead if it either:
	- Travels its full distance of 5 squares
	- If it makes contact with a Protester, Boulder, or dirt
	- Or if its about to go out-of-bounds
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
