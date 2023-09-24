#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
class StudentWorld;

const int WAITING_TO_ROLL = 0;
const int WALKING = 1;

// not sure if we need default construcotrs
class Actor : public GraphObject
{ //TODO: add functions of base class
    public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size): GraphObject(imageID, startX, startY, startDirection, depth, size)
    {
        m_world = world;
        m_active = true;
    }
        //virtual ~Actor(); //TODO: do i need dynamic allocation or nah
    
        StudentWorld* world();
        bool overlap(Actor* other); //TODO: this
        bool completeOverlap(Actor* other);
        virtual void doSomething() = 0;
        bool isActive();
    
    private:
        bool m_active;
        StudentWorld* m_world;
};

class Moveable : public Actor
{
    public:
    Moveable(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size): Actor(world, imageID, startX, startY, startDirection, depth, size) {}
    
    bool canMoveForward(int dir);
    void chooseFork(int dir);
    bool hasChosenFork();
    bool outOfBounds();
    bool alignsWithSquare();
    bool atFork();
    int getState();
    int getWalkDir();
    int getTicksToMove();
    void setState(int state);
    void setWalkDir(int dir);
    void setTicksToMove(int ticks);
    
    private:
        int m_state;
        int m_walkDir;
        int m_ticksToMove;
        bool m_chosenFork = false;

};

class Square : public Actor
{
    public:
        Square(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size): Actor(world, imageID, startX, startY, startDirection, depth, size) {}
        void doSomething();
};

class Vortex : public Moveable
{
    public: //TODO: are startX and startY doubles or ints??
        Vortex(StudentWorld* world, int imageID, int startX, int startY, int startDirection, int depth, double size): Moveable(world, imageID, startX, startY, startDirection, depth, size) {}
};

class Player : public Moveable
{
    public:
    // deal with Yoshi by passing in diff imageID
    Player(int playerNum, StudentWorld* world, int imageID, int startX, int startY);
    void doSomething();
    int getPlayerNum();
    int getStars();
    // void chooseFork(int dir);
    void setStars(int stars);
    int getCoins();
    void setCoins(int coins);
    Vortex* vortex();
    
    private:
        int m_playerNum;
        int m_stars;
        int m_coins;
        Vortex* m_vortex;
        bool invalidDir();
};

class Baddie : public Moveable
{
};

//TODO: Implement coin squares
class CoinSquare: public Square
{
    public:
        CoinSquare(StudentWorld* world, int imageID, int startX, int startY);
        void doSomething();
        void modifyCoins(Player* p);
        
    private:
        bool m_gavePeachCoins;
        bool m_gaveYoshiCoins;
    
    
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
