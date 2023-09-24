#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"

class StudentWorld;
class Player;

const int WAITING = 0;
const int WALKING = 1;
const int BLUE = 2;
const int RED = 3;
const int INVALID_DIR = 6;
const int FORK = 7;
const int CORNER = 8;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// Actor
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size) :  GraphObject(imageID, startX, startY, startDirection, depth, size), m_active(true), m_state(WAITING), m_world(world), m_peachNew(true), m_yoshiNew(true) {}
    virtual ~Actor() {}
    StudentWorld* world() const;
    bool isActive() const; // Check whether an Actor is active
    void setActiveStatus(bool status); // Set an Actor as either active or inactive
    virtual bool isMovable() const; // Check if an actor can move
    virtual bool impactable() const; //
    bool align(Actor* other) const; // Check if an actor aligns with another actor
    bool movedOnto(Actor* p) const; // Check if an actor has passed over another
    bool landedOn(Actor* p) const; // Check if an actor has landed on another
    bool intersectAtPos(int x, int y) const; // Check if an actor intersects with another by checking if it is in range of a certain position
    virtual void doSomething() = 0;
    virtual void impact(Player* p) {} // Perform a specialized action on players
    int getState() const; // Get the actor's state
    void setState(int state); // Set the actor's state
    bool playerIsNew(int playerNum) const; // Check whether players have been encountered relative to the actor
    void setPlayerNew(bool status, int playerNum); // Set a player to encountered or unencountered
    void resetPlayers(); // Reset all players back to unencountered once they move away
    virtual bool vortexImpact(int x, int y); // Actions an actor performs if hit by a vortex
    
private:
    bool m_active;
    int m_state;
    StudentWorld* m_world;
    bool m_peachNew;
    bool m_yoshiNew;
};

// Squares
class Square : public Actor
{
    public:
        Square(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size)
            : Actor(world, imageID, startX, startY, startDirection, depth, size) {}
    bool isMovable() const; // Squares can't move
    int deductCoins(Player* p, int amt); // Take away a certain amount of coins from the player, but less if they do not have up to that amount
    void doSomething();
};

// Coin square
class CoinSquare: public Square
{
    public:
        CoinSquare(StudentWorld* world,int startX, int startY, int color) : Square(world, color==BLUE ? IID_BLUE_COIN_SQUARE : IID_RED_COIN_SQUARE, startX, startY, right, 1, 1), m_color(color) {}
        void impact(Player* p);
            
    private:
        int m_color;
};

// Star square
class StarSquare : public Square
{
    public:
        StarSquare(StudentWorld* world, int startX, int startY)
            : Square(world, IID_STAR_SQUARE, startX, startY, right, 1, 1)  { }
        void impact(Player* p);
};

// Directional square
class DirectionalSquare : public Square
{
    public:
        DirectionalSquare(StudentWorld* world, int startX, int startY, int dir)
            : Square(world, IID_DIR_SQUARE, startX, startY, dir, 1, 1) { }
        void impact(Player* p);
};

// Bank square
class BankSquare : public Square
{
    public:
        BankSquare(StudentWorld* world, int startX, int startY)
            : Square(world, IID_BANK_SQUARE, startX, startY, right, 1, 1) { }
        void impact(Player* p);
};

// Event square
class EventSquare : public Square
{
    public:
        EventSquare(StudentWorld* world, int startX, int startY)
            : Square(world, IID_EVENT_SQUARE, startX, startY, right, 1, 1) { }
        void impact(Player* p);
};

// Dropping square
class DroppingSquare : public Square
{
    public:
        DroppingSquare(StudentWorld* world, int startX, int startY)
            : Square(world, IID_DROPPING_SQUARE, startX, startY, right, 1, 1) { }
        void impact(Player* p);
};

// Movable actors
class Movable : public Actor
{
public:
    Movable(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size)
        : Actor(world, imageID, startX, startY, right, depth, size), m_walkDir(right), m_ticksToMove(0) {}
    
    int getWalkDir() const; // Get a movable actor's walk direction
    void setWalkDir(int dir); // Set the walking direction of a movable actor
    int getTicksToMove() const; // Retrieve ticks to move
    void setTicksToMove(int ticks); // Change ticks to move
    bool canMoveForward(int dir) const; // Check whether a movable actor can go forward
    bool outOfBounds() const; // Check if movable object is out of bounds of the entire screen
    bool directlyOnSquare() const; // Check if a movable object is directly on top of a square
    int availablePositions() const; // See how many available positions are available to move to
    int backward() const; // Find the direction the actor is coming from and cannot move to if at a fork
    bool atFork() const; // See whether a movable actor is at a fork
    bool atCorner() const; // See whether a movable actor is at a corner
    bool turn(); // Turn to face a perpendicular direction
    int randomDir(); // Choose a random valid walk direction
    void walkForward(int pixels); // Choose a random valid walk direction
    void teleport(); // Teleport to another valid square
    
private:
    int m_walkDir;
    int m_ticksToMove;
};

// Enemies
class Enemy : public Movable
{
    public:
        Enemy(StudentWorld* world, int imageID, int startX, int startY) : Movable(world, imageID, startX, startY, right, 0, 1), m_pauseCounter(180), m_squaresToMove(0) {}
        virtual bool impactable() const; // Enemies can be hit by a vortex
        void doSomething();
        bool vortexImpact(int x, int y); // Teleport if hit by vortex
        virtual bool squareImpactAllowed(); // Whether the enemy can impact squares

    private:
        int m_pauseCounter;
        int m_squaresToMove;
};


// Bowser
class Bowser : public Enemy
{
    public:
        Bowser(StudentWorld* world, int startX, int startY) : Enemy(world, IID_BOWSER, startX, startY) {}
        void doSomething();
        void impact(Player* p); // 50% chance of player losing all coins and stars
        bool squareImpactAllowed(); // Only Bowser can impact squares
};

// Boo
class Boo : public Enemy
{
    public:
        Boo(StudentWorld* world, int startX, int startY) : Enemy(world, IID_BOO, startX, startY) {}
        void impact(Player* p);
        void swapCoins(Player* p); // Swap coins
        void swapStars(Player* p); // Swap stars
};

// Vortex
class Vortex : public Movable
{
    public:
    // Vortex
        Vortex(StudentWorld* world, int startX, int startY, int depth, double size, int dir);
        void setInUse(bool status); // Set a vortex as being used by a player
        void doSomething();
    private:
        bool m_using;
};

// Player
class Player : public Movable
{
public:
    Player(int playerNum, StudentWorld* world, int imageID, int startX, int startY) : Movable (world, imageID, startX, startY, right, 0, 1), m_playerNum(playerNum), m_stars(0), m_coins(0), m_onDirSquare(false), m_swapped(false), m_swappedCoins(false), m_swappedStars(false), m_hasVortex(false), m_vortex(nullptr) {}
    int getPlayerNum() const; // Get the player number
    Player* otherPlayer() const; // Get a pointer to the other player
    int getCoins() const; // Get number of coins
    void setCoins(int coins); // Set number of coins
    int getStars() const; // Get number of stars
    void setStars(int stars); // Change number of stars
    void setOnDirSquare(bool status); // Mark whether a player is on a directional square
    void doSomething();
    void swap(); // Swap a player's position with the other player's
    bool swapped() const; // Check if a player's position has been swapped
    void setSwapped(bool status); // Mark a player as having just swapped its position
    bool swappedCoins() const; // Check if a player's coins have been swapped
    void setSwappedCoins(bool status); // Mark a player as having just swapped its coins
    bool swappedStars() const; // Check if a player's stars have been swapped
    void setSwappedStars(bool status); // Mark a player as having just swapped its stars
    Vortex* vortex() const; // Access a player's vortex
    bool hasVortex() const; // See if a player has a vortex
    void createVortex(); // Give a player a vortex
    
private:
        int m_playerNum;
        int m_stars;
        int m_coins;
        bool m_onDirSquare;
        bool m_swapped;
        bool m_swappedCoins;
        bool m_swappedStars;
        bool m_hasVortex;
        Vortex* m_vortex;
};

#endif // ACTOR_H_
