#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor
StudentWorld* Actor::world() const { return m_world; } // Return a pointer to the world the Actor is in

bool Actor::isActive() const { return m_active; } // Check whether an Actor is active
void Actor::setActiveStatus(bool status) { m_active = status; } // Set an Actor as either active or inactive
bool Actor::isMovable() const { return true; } // Check if an actor can move
bool Actor::impactable() const { return false; } // Check if an actor can be impacted by a vortex
bool Actor::align(Actor* other) const { return (getX() == other->getX() && getY() == other->getY()); }  // Check if an actor aligns with another actor
bool Actor::movedOnto(Actor* p) const { return align(p) && p->getState() == WALKING; } // Check if an actor has passed over another
bool Actor::landedOn(Actor* p) const { return align(p) && p->getState() == WAITING; } // Check if an actor has landed on another

// Check if an actor intersects with another by checking if it is in range of a certain position
bool Actor::intersectAtPos(int x, int y) const
{
    x = getX() - x;
    y = getY() - y;
    x = (x > 0) ? x : -x;
    y = (y > 0) ? y : -y;
    return ((x < SPRITE_WIDTH) && (y < SPRITE_HEIGHT));
}

int Actor::getState() const { return m_state; } // Get the actor's state
void Actor::setState(int state) { m_state = state; } // Set the actor's state
bool Actor::playerIsNew(int playerNum) const {return (playerNum == 1) ? m_peachNew : m_yoshiNew; } // Check whether players have been encountered relative to the actor

// Set a player to encountered or unencountered
void Actor::setPlayerNew(bool status, int playerNum)
{
    if (playerNum == 1) m_peachNew = status;
    else m_yoshiNew = status;
}

// Reset all players back to unencountered once they move away
void Actor::resetPlayers()
{
    for (int i = 1; i <= PLAYER_COUNT; i++)
    {
        if (world()->player(i))
        {
            if (world()->player(i)->getState() == WALKING || (world()->player(i)->getX() != getX() || world()->player(i)->getY() != getY())) // Either player moved or the activating actor, if it's an enemy, moved
            {
                setPlayerNew(true, i);
                    world()->player(1)->setSwapped(false);
                    world()->player(1)->setSwappedCoins(false);
                    world()->player(1)->setSwappedStars(false);
                world()->player(2)->setSwapped(false);
                world()->player(2)->setSwappedCoins(false);
                world()->player(2)->setSwappedStars(false);
            }
        }
    }
}

bool Actor::vortexImpact(int x, int y) { return false; } // Actions an actor performs if hit by a vortex

// Squares

bool Square::isMovable() const { return false; } // Squares can't move

// Take away a certain amount of coins from the player, but less if they do not have up to that amount
int Square::deductCoins(Player* p, int amt)
{
    if (p)
    {
        int amtDeducted;
        if (p->getCoins() >= amt) amtDeducted = amt;
        else amtDeducted = p->getCoins();
        p->setCoins(p->getCoins()-amtDeducted);
        return amtDeducted;
    }
    else return -1;
}

void Square::doSomething()
{
    resetPlayers(); // Reset players before affecting them
        
    for (int i = 1; i <= PLAYER_COUNT; i++)
    {
        if (world()->player(i) && playerIsNew(i)) impact(world()->player(i)); // Only affect new players
    }
    return;
}

// Coin square
void CoinSquare::impact(Player* p)
{
    if (!isActive()) return;
    
    if (p && landedOn(p))
    {
        if (m_color == BLUE) // Blue coin squares give coins
        {
            p->setCoins(p->getCoins()+3);
            world()->playSound(SOUND_GIVE_COIN);
        }
        else if (m_color == RED) // Red coin squares take away coins
        {
            deductCoins(p, 3);
            world()->playSound(SOUND_TAKE_COIN);
        }
            
        setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
    }
}

// Star square
void StarSquare::impact(Player* p)
{
    if (p && align(p))
    {
        if (p->getCoins() < 20) return; // Do nothing if player doesn't have enough coins
        else
        {
            deductCoins(p, 20); // Take away coins and give a star
            p->setStars(p->getStars()+1);
            world()->playSound(SOUND_GIVE_STAR);
        }
        setPlayerNew(false, p->getPlayerNum());
    }
}

// Directional square
void DirectionalSquare::impact(Player* p)
{
    if (p && align(p))
    {
        p->setOnDirSquare(true);
        p->setWalkDir(getDirection()); // Change the walk direction
        if (p->getTicksToMove() > 0) // Make player walk in forcing direction
        {
            p->walkForward(2);
            if (p->getTicksToMove() > 0) p->setTicksToMove(p->getTicksToMove()-1);
            else p->setState(WAITING);
        }
        p->setOnDirSquare(false); // Player must resume normal behavior after moving off
        setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
    }
}

// Bank square
void BankSquare::impact(Player* p)
{
    if (p)
    {
        if (landedOn(p)) // Give coins if landed on
        {
            p->setCoins(p->getCoins()+world()->bankBalance());
            world()->resetBank();
            world()->playSound(SOUND_WITHDRAW_BANK);
            setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
        }
        else if (movedOnto(p)) // Add coins to bank if just passed over
        {
            world()->addToBank(deductCoins(p, 5));
            world()->playSound(SOUND_DEPOSIT_BANK);
            setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
        }
    }
}

// Event square
void EventSquare::impact(Player* p)
{
    if (p && landedOn(p) && (!world()->player(1)->swapped() && !world()->player(2)->swapped())) // If players have just been swapped, do not activate
    {
        switch (randInt(0, 2)) // Randomly teleport, swap, or give a vortex to the player
        {
           case 0:
                p->teleport();
                p->setWalkDir(INVALID_DIR);
                world()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 1:
                p->swap();
                world()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            case 2:
                p->createVortex();
                world()->playSound(SOUND_GIVE_VORTEX);
                break;
        }
        setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
    }
}

// Dropping square
void DroppingSquare::impact(Player* p)
{
    if (p && landedOn(p))
    {
        switch (randInt(0,1))
        {
            case 0:
                deductCoins(p, 10); // Take away coins
                break;
            case 1:
                if (p->getStars() >= 1) // Take away a star
                    p->setStars(p->getStars()-1);
                break;
        }
        world()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        setPlayerNew(false, p->getPlayerNum()); // Mark player as encountered
    }
}

// Movable actors
int Movable::getWalkDir() const { return m_walkDir; } // Get a movable actor's walk direction

// Set the walking direction of a movable actor
void Movable::setWalkDir(int dir)
{
    m_walkDir = dir;
    if (m_walkDir == left) setDirection(left);
    else setDirection(right);
}

int Movable::getTicksToMove() const { return m_ticksToMove; } // Retrieve ticks to move
void Movable::setTicksToMove(int ticks) { m_ticksToMove = ticks; } // Change ticks to move

// Check whether a movable actor can go forward
bool Movable::canMoveForward(int dir) const
{
    switch (dir)
    {
        case right:
            return (world()->board().getContentsOf(getX()/SPRITE_WIDTH+1, getY()/SPRITE_HEIGHT) != Board::empty);
        case left:
            return (world()->board().getContentsOf(getX()/SPRITE_WIDTH-1, getY()/SPRITE_HEIGHT) != Board::empty);
        case up:
            return (world()->board().getContentsOf(getX()/SPRITE_WIDTH, (getY()+SPRITE_HEIGHT)/SPRITE_HEIGHT) != Board::empty);
        case down:
            return (world()->board().getContentsOf(getX()/SPRITE_WIDTH, (getY()-SPRITE_HEIGHT)/SPRITE_HEIGHT) != Board::empty);
    }
    
    return false;
}

// Check if movable object is out of bounds of the entire screen
bool Movable::outOfBounds() const { return getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT; }

// Check if a movable object is directly on top of a square
bool Movable::directlyOnSquare() const { return (getX()%SPRITE_WIDTH == 0 && getY()%SPRITE_HEIGHT == 0); }

// See how many available positions are available to move to
int Movable::availablePositions() const
{
    int open = 0;
    
    if (canMoveForward(right)) open++;
    if (canMoveForward(left)) open++;
    if (canMoveForward(up)) open++;
    if (canMoveForward(down)) open++;
    
    return open;
}

// Find the direction the actor is coming from and cannot move to if at a fork
int Movable::backward() const
{
    int backward = right;
    
    if (getWalkDir() == right) backward = left;
    if (getWalkDir() == left) backward = right;
    if (getWalkDir() == up) backward = down;
    if (getWalkDir() == down) backward = up;
    
    return backward;
}

bool Movable::atFork() const { return (availablePositions() > 2); } // See whether a movable actor is at a fork
bool Movable::atCorner() const { return (availablePositions() == 2); } // See whether a movable actor is at a corner

// Turn to face a perpendicular direction
bool Movable::turn()
{
    if (m_walkDir == right || m_walkDir == left)
    {
        if (canMoveForward(up)) setWalkDir(up); // Prioritize going up
        else if (canMoveForward(down)) setWalkDir(down);
        return true;
    }
    else if (m_walkDir == up || m_walkDir == down)
    {
        if (canMoveForward(right)) setWalkDir(right); // Prioritize going right
        else if (canMoveForward(left)) setWalkDir(left);
        return true;
    }
    return false;
}

// Choose a random valid walk direction
int Movable::randomDir()
{
    int ind = randInt(0,3);
    int dirs[4] = {right, left, up, down};
    int target = right;
    int count = 0;
    do
    {
        target = dirs[ind];
        ind++;
        count++;
        if (ind > 3) ind = 0;
    } while (count < 4 && !canMoveForward(target));
    return target;
}

// Move forward a certain number of pixels
void Movable::walkForward(int pixels)
{
    switch (m_walkDir) {
        case left:
            moveTo(getX() - pixels, getY());
            break;
        case right:
            moveTo(getX() + pixels, getY());
            break;
        case up:
            moveTo(getX(), getY() + pixels);
            break;
        case down:
            moveTo(getX(), getY() - pixels);
            break;
        default:
            break;
    }
    return;
}

// Teleport to another valid square
void Movable::teleport()
{
    int x, y;
    do
    {
        x = randInt(0, SPRITE_WIDTH);
        y = randInt(0, SPRITE_HEIGHT);
        
    } while (world()->board().getContentsOf(x, y) == Board::empty || (x == getX()/SPRITE_WIDTH && y == getY()/SPRITE_HEIGHT));
    
    x *= SPRITE_WIDTH;
    y *= SPRITE_HEIGHT;
    moveTo(x, y);
}

// Enemies
bool Enemy::impactable() const { return true; } // Enemies can be hit by a vortex

void Enemy::doSomething()
{
    resetPlayers(); // Mark players as not encountered anymore once they move off
    bool activatedOnce = false;
    
    if (getState() == WAITING)
    {
        for (int i = 1; i <= PLAYER_COUNT; i++)
        {
            if (world()->player(i) && world()->player(i)->otherPlayer())
            {
                // Players must not have just had their currency swapped by Boo
                bool currencyNotSwapped = (!world()->player(i)->swappedCoins() && !world()->player(i)->otherPlayer()->swappedCoins()) && (!world()->player(i)->swappedStars() && !world()->player(i)->otherPlayer()->swappedStars());
                
                if (landedOn(world()->player(i)) && playerIsNew(i) && currencyNotSwapped)
                {
                    impact(world()->player(i));
                    setPlayerNew(false, i);
                    activatedOnce = true;
                }
                if (activatedOnce) break; // Enemies may only activate on one player at a time
            }
        }
        
        m_pauseCounter--; // Decrement pause counter
        
       if (m_pauseCounter <= 0) // Make enemy move again
       {
           int upperLimit = squareImpactAllowed() ? 10 : 3;
           m_squaresToMove = randInt(1, upperLimit);
           setTicksToMove(m_squaresToMove*8);
           setWalkDir(randomDir());
           setState(WALKING);
       }
    }
    else if (getState() == WALKING) // Move across a random number of squares if enough ticks left
    {
        if (directlyOnSquare())
        {
            if (atFork())  setWalkDir(randomDir());
            else if (atCorner() && !canMoveForward(getWalkDir())) turn();
        }
        if (getTicksToMove() > 0)
        {
            walkForward(2);
            setTicksToMove(getTicksToMove() - 1);
        }
        else
        {
            setState(WAITING);
            m_pauseCounter = 180;
        }
    }
}

// Teleport if hit by vortex
bool Enemy::vortexImpact(int x, int y)
{
    if (intersectAtPos(x, y))
    {
        teleport();
        setState(WAITING);
        setTicksToMove(180);
        return true;
    }
    return false;
}

bool Enemy::squareImpactAllowed() { return false; } // Whether the enemy can impact squares

// Bowser
void Bowser::impact(Player* p)
{
    if (p && randInt(0,1)) // 50% chance of player losing all coins and stars
    {
        world()->player(p->getPlayerNum())->setCoins(0);
        world()->player(p->getPlayerNum())->setStars(0);
        world()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::doSomething()
{
   Enemy::doSomething();
   if (getState() == WALKING && getTicksToMove() <= 0)
   {
       if (getTicksToMove() <= 0) // 25% chance of making a dropping square
       {
           if (!randInt(0,3))
           {
                Actor* d = new DroppingSquare(world(), getX(), getY());
                world()->replaceSquare(this, d);
                world()->playSound(SOUND_DROPPING_SQUARE_CREATED);
           }
       }
   }
}

bool Bowser::squareImpactAllowed() { return true; } // Only Bowser can impact squares

// Boo
void Boo::impact(Player* p)
{
    if (p)
    {
        if (randInt(0,1)) swapCoins(p); // Swap either coins or stars
        else swapStars(p);
        world()->playSound(SOUND_BOO_ACTIVATE);
    }
}

// Swap coins
void Boo::swapCoins(Player* p)
{
    if (p && p->otherPlayer())
    {
        Player* other = p->otherPlayer();
        
        if (!p->swappedCoins() && !other->swappedCoins()) // Ensure that Boo does not infinitely swap and can mark players as encountered
        {
            int tempCoins = p->getCoins();
            p->setCoins(other->getCoins());
            other->setCoins(tempCoins);
            
            p->setSwappedCoins(true);
            other->setSwappedCoins(true);
        }
    }
}

// Swap stars
void Boo::swapStars(Player* p)
{
    if (p && p->otherPlayer())
    {
        Player* other = p->otherPlayer();
        
        if (!p->swappedStars() && !other->swappedStars()) // Ensure that Boo does not infinitely swap and can mark players as encountered
        {
            int tempStars = p->getStars();
            p->setStars(other->getStars());
            other->setStars(tempStars);
            
            p->setSwappedStars(true);
            other->setSwappedStars(true);
        }
    }
}

// Vortex
Vortex::Vortex(StudentWorld* world, int startX, int startY, int depth, double size, int dir) : Movable(world, IID_VORTEX, startX, startY, right, 0, 1) { setWalkDir(dir); }

void Vortex::setInUse(bool status) { m_using = status; } // Set a vortex as being used by a player

void Vortex::doSomething()
{
    if (!isActive()) return;
    
        if (m_using)
        { // Fire vortex
            walkForward(2);
            if (outOfBounds())
            { // Set to inactive if out of bounds
                setActiveStatus(false);
                return;
            }
            if (world()->checkVortexImpact(getX(), getY())) // If the vortex has overlapped with an enemy
            {
                setActiveStatus(false); // Set inactive
                world()->playSound(SOUND_HIT_BY_VORTEX);
                return;
            }
        }
}

// Player
int Player::getPlayerNum() const { return m_playerNum; } // Get the player number

// Get a pointer to the other player
Player* Player::otherPlayer() const
{
    if (m_playerNum == 1) return world()->player(2);
    else return world()->player(1);
}

int Player::getCoins() const { return m_coins; } // Get number of coins
void Player::setCoins(int coins) { m_coins = coins; } // Set number of coins
int Player::getStars() const { return m_stars; } // Get number of stars
void Player::setStars(int stars) { m_stars = stars; } // Change number of stars
void Player::setOnDirSquare(bool status) { m_onDirSquare = status; } // Mark whether a player is on a directional square

void Player::doSomething()
{
    if (m_onDirSquare) return; // Let directional squares change the walking direction to their forcing direction
    
    if (getState() == WAITING)
    {
        if (getWalkDir() == INVALID_DIR)
            setWalkDir(randomDir()); // Pick a random valid direction if just teleported
        
            switch (world()->getAction(m_playerNum)) // Get input
            {
                case ACTION_ROLL: // Start walking
                {
                    int die_roll = randInt(1,10);
                    setTicksToMove(die_roll*8);
                    setState(WALKING);
                    break;
                }
                case ACTION_FIRE: // Fire a vortex if the player has one
                {
                    if (m_hasVortex)
                    {
                        m_vortex = new Vortex(world(), getX(), getY(), 0, 1, getWalkDir());
                        world()->newActor(m_vortex); // Add the vortex to a vector of actors
                        m_vortex->setInUse(true);
                        world()->playSound(SOUND_PLAYER_FIRE);
                        m_vortex = nullptr; // Mark the vortex as used
                        m_hasVortex = false;
                    }
                }
                default:
                    return;
            }
    }
    else
    {
        bool move = true; // To allow players to stop at forks
          
        if (directlyOnSquare()) // Only turn or choose a position if directly on a square
        {
            if (atFork())
            {
                switch (world()->getAction(m_playerNum)) // Let the user pick a direction if at a fork
                {
                    case ACTION_RIGHT:
                        if (canMoveForward(right) && backward() != right) setWalkDir(right);
                        else move = false;
                        break;
                    case ACTION_LEFT:
                        if (canMoveForward(left) && backward() != left) setWalkDir(left);
                        else move = false;
                        break;
                    case ACTION_UP:
                        if (canMoveForward(up) && backward() != up) setWalkDir(up);
                        else move = false;
                        break;
                    case ACTION_DOWN:
                        if (canMoveForward(down) && backward() != down) setWalkDir(down);
                        else move = false;
                        break;
                    default:
                        move = false;
                        break;
                }
            }
            else {
                if (atCorner() && !canMoveForward(getWalkDir())) move = turn(); // If can't move in current direction, pick a perpendicular direction
            }
        }
        if (getTicksToMove() > 0) { // If still able to move, then move and decrement ticks
            if (move) {
                walkForward(2);
                setTicksToMove(getTicksToMove() - 1);
            }
        }
        else {
            setState(WAITING); // Roll has been used up
        }
    }
}

// Swap a player's position with the other player's
void Player::swap()
{
    if (otherPlayer())
    {
        Player* other = otherPlayer();
        
        if (!swapped() && !other->swapped())
        {
            int tempX = getX();
            int tempY = getY();
            moveTo(other->getX(), other->getY());
            other->moveTo(tempX, tempY);
            
            int tempTicks = getTicksToMove();
            setTicksToMove(other->getTicksToMove());
            other->setTicksToMove(tempTicks);
            
            int tempWalkDir = getWalkDir();
            setWalkDir(other->getWalkDir());
            other->setWalkDir(tempWalkDir);
            
            int tempSpriteDir = getDirection();
            setDirection(other->getDirection());
            other->setDirection(tempSpriteDir);
            
            int tempState = getState();
            setState(other->getState());
            other->setState(tempState);
        }
    }
}

bool Player::swapped() const { return m_swapped; } // Check if a player's position has been swapped
void Player::setSwapped(bool status) { m_swapped = status; } // Mark a player as having just swapped its position
bool Player::swappedCoins() const { return m_swappedCoins; } // Check if a player's coins have been swapped
void Player::setSwappedCoins(bool status) { m_swappedCoins = status; } // Mark a player as having just swapped its coins
bool Player::swappedStars() const { return m_swappedStars; } // Check if a player's stars have been swapped
void Player::setSwappedStars(bool status) { m_swappedStars = status; } // Mark a player as having just swapped its stars
Vortex* Player::vortex() const { return m_vortex; } // Access a player's vortex
bool Player::hasVortex() const { return m_hasVortex; } // See if a player has a vortex

// Give a player a vortex
void Player::createVortex()
{
    if (!m_vortex) m_hasVortex = true;
}
