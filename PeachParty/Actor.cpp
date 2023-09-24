#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor
StudentWorld* Actor::world()
{
    return m_world;
}

bool Actor::isActive()
{
    return m_active;
}

// check if two actors have the exact same coordinates
bool Actor::completeOverlap(Actor* other)
{
    if (getX() == other->getX() && getY() == other->getY())
        return true;
    return false;
}


// See if a moveable thing is out of bounds
bool Moveable::outOfBounds()
{
    return (getX()/SPRITE_WIDTH < 0 || getX()/SPRITE_WIDTH >= BOARD_WIDTH || getY()/SPRITE_HEIGHT < 0 || getY()/SPRITE_HEIGHT >= BOARD_HEIGHT);
}

bool Moveable::alignsWithSquare()
{
    return (getX()%SPRITE_WIDTH == 0 && getY()%SPRITE_HEIGHT == 0);
}

// Moveable
bool Moveable::canMoveForward(int dir)
{
    if (outOfBounds()) return false;
    
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
    
    return false; // invalid direction
}

/*bool Moveable::movedOntoSquare()
{
    
}*/

// check whether a Moveable can move in more than one direction
bool Moveable::atFork()
{
    if (world()->board().getContentsOf(getX()/SPRITE_WIDTH, getY()/SPRITE_HEIGHT) == Board::player) return false; // the starting position is never a fork location
    if (!alignsWithSquare()) return false; // no UB
    
    Board::GridEntry dirs[4];
    
    dirs[0] = world()->board().getContentsOf(getX()/SPRITE_WIDTH+1, getY()/SPRITE_HEIGHT);
    dirs[1] = world()->board().getContentsOf(getX()/SPRITE_WIDTH-1, getY()/SPRITE_HEIGHT);
    dirs[2] = world()->board().getContentsOf(getX()/SPRITE_WIDTH, getY()/SPRITE_HEIGHT+1);
    dirs[3] = world()->board().getContentsOf(getX()/SPRITE_WIDTH, getY()/SPRITE_HEIGHT-1);
        
    int open = 0;
    
    for (int i = 0; i < 4; i++)
    {
        if (dirs[i] != Board::empty)
            open++;
    }
    
    return (open > 2);
}

bool Moveable::hasChosenFork()
{
    return m_chosenFork;
}

// choose a direction at a fork
void Moveable::chooseFork(int dir)
{
    if (getState() == WALKING) m_chosenFork = false;
    
        switch(dir)
        {
            case right:
            {
                if (alignsWithSquare() && canMoveForward(right))
                {
                    m_chosenFork = true;
                    setWalkDir(right);
                    setDirection(right);
                    setState(WALKING);
                }
                break;
            }
            case left:
            {
                if (alignsWithSquare() && canMoveForward(left))
                {
                    m_chosenFork = true;
                    setWalkDir(left);
                    setDirection(left);
                    setState(WALKING);
                }
                break;
            }
            case up:
            {
                if (alignsWithSquare() && canMoveForward(up))
                {
                    m_chosenFork = true;
                    setWalkDir(up);
                    setDirection(right);
                    setState(WALKING);
                }
                break;
            }
            case down:
            {
                if (alignsWithSquare() && canMoveForward(down))
                {
                    m_chosenFork = true;
                    setWalkDir(up);
                    setDirection(right);
                    setState(WALKING);
                }
                break;
            }
        }
}



int Moveable::getState()
{
    return m_state;
}

int Moveable::getWalkDir()
{
    return m_walkDir;
}

int Moveable::getTicksToMove()
{
    return m_ticksToMove;
}

void Moveable::setState(int state)
{
    m_state = state;
}

void Moveable::setWalkDir(int dir)
{
    m_walkDir = dir;
}

void Moveable::setTicksToMove(int ticks)
{
    m_ticksToMove = ticks;
}

Player::Player(int playerNum, StudentWorld* world, int imageID, int startX, int startY) : Moveable(world, imageID, startX, startY, right, 0, 1)
{
    m_playerNum = playerNum;
    setState(WAITING_TO_ROLL);
    setTicksToMove(0);
    setWalkDir(right);
    m_stars = 0;
    m_coins = 0;
}

// has the player been teleported?
bool Player::invalidDir()
{
    if ((getWalkDir() != right) && (getWalkDir() != left) && (getWalkDir() != up) && (getWalkDir() != down))
        return true;
    else return false;
}

// mess lmao
//TODO: this may get repeated for other Moveables
void Player::doSomething()
{
 // if ((getX()%SPRITE_WIDTH == 0 && getY()%SPRITE_HEIGHT == 0) && atFork()) setState(WAITING_TO_ROLL);

    if (getState() == WAITING_TO_ROLL)
    {
            switch (world()->getAction(m_playerNum))
            {
                case ACTION_ROLL:
                {
                    if (alignsWithSquare() && !atFork())
                    {
                        int die_roll = randInt(1,10);
                        setTicksToMove(die_roll*8);
                        setState(WALKING);
                    }
                    break;
                }
                case ACTION_FIRE:
                {
                    //
                }
                default:
                    return;
            }
        
        
        if (getTicksToMove() <= 0)
            setState(WAITING_TO_ROLL);
    }
    else // Player is walking
    {        //if can't continue moving forward in current direction
        if (atFork())
        {
            if (!hasChosenFork()) setState(WAITING_TO_ROLL);
            
            switch (world()->getAction(m_playerNum))
            {
                case ACTION_RIGHT:
                {
                    chooseFork(right);
                    break;
                }
                case ACTION_LEFT:
                {
                    chooseFork(left);
                    break;
                }
                case ACTION_UP: // what if it's backwards lol
                {
                    chooseFork(up);
                    break;
                }
                case ACTION_DOWN:
                {
                    chooseFork(down);
                    break;
                }
                default:
                {
                    break;
                }
                
            }
        }
            
    }
        
        if (alignsWithSquare() && !canMoveForward(getWalkDir())) // only check if DIRECTLY on a square
        {
            // TODO: other Moveables may need this, maybe put this into a Moveable function
            if (getWalkDir() == right || getWalkDir() == left)
            {
                if (canMoveForward(up)) // prefer upward direction
                {
                    setWalkDir(up);
                    setDirection(right); // it could have started moving left
                    moveTo(getX(), getY()+2); // move up 2
                }
                else if (canMoveForward(down))
                {
                    setWalkDir(down);
                    setDirection(right); // it could have started moving left
                    moveTo(getX(), getY()-2); // move down 2
                }
            }
            else if (getWalkDir() == up || getWalkDir() == down)
            {
                if (canMoveForward(right))
                {
                    setWalkDir(right);
                    moveTo(getX()+2, getY());
                }
                else if (canMoveForward(left))
                {
                    setWalkDir(left);
                    setDirection(left); // change direction
                    moveTo(getX()-2, getY());
                }
            }
            //TODO: not sure about this
        }
        else { // can move forward
            switch (getWalkDir())
            {
                case right:
                {
                    moveTo(getX()+2, getY());
                    break;
                }
                case left:
                {
                    moveTo(getX()-2, getY());
                    break;
                }
                case up:
                {
                    moveTo(getX(), getY()+2);
                    break;
                }
                case down:
                {
                    moveTo(getX(), getY()-2);
                    break;
                }
            }
        }
        
        setTicksToMove(getTicksToMove()-1);
        
        if (getTicksToMove() <= 0)
            setState(WAITING_TO_ROLL);
    
}

void Square::doSomething()
{
    
}

int Player::getCoins()
{
    return m_coins;
}

int Player::getStars()
{
    return m_stars;
}

void Player::setCoins(int coins)
{
    m_coins = coins;
}

void Player::setStars(int stars)
{
    m_stars = stars;
}

CoinSquare::CoinSquare(StudentWorld* world, int imageID, int startX, int startY) : Square(world, imageID, startX, startY, 0, 1, 1)
{
    m_gavePeachCoins = false;
    m_gaveYoshiCoins = false;
}

void CoinSquare::modifyCoins(Player* p)
{
    p->setCoins(p->getCoins()+3);
}

void CoinSquare::doSomething() //TODO: this thing whatever it does
{
    if (isActive())
    {
        if (world()->peach()->getState() == WALKING)
            m_gavePeachCoins = false;
        
        if (world()->yoshi()->getState() == WALKING)
            m_gaveYoshiCoins = false;
        
        if (completeOverlap(world()->peach()) && world()->peach()->getState() == WAITING_TO_ROLL)
        {
            if (!m_gavePeachCoins) modifyCoins(world()->peach());
            m_gavePeachCoins = true;
        }
        if (completeOverlap(world()->yoshi()) && world()->yoshi()->getState() == WAITING_TO_ROLL)
        {
            if (!m_gaveYoshiCoins) modifyCoins(world()->yoshi());
            m_gaveYoshiCoins = true;
        }
    }
    return;
}
