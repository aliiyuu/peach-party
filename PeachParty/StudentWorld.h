#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include <list>
#include "Board.h"
#include <string>
class Actor;
class Player;

const int PLAYER_COUNT = 2;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
    public:
        StudentWorld(std::string assetPath);
        ~StudentWorld();
        Board board(); // Access the board
        virtual int init(); // Initialize the game
        virtual int move(); // Update the board
        virtual void cleanUp(); // Delete all actors
        void newActor(Actor* a); // Allocate a new non-player actor
        void removeActor(Actor* a); // Delete an actor during the game
        Player* player(int playerNum); // Return a pointer to a player
        void replaceSquare(Actor* agent, Actor* newSquare); // Delete a square at the current position of a new (dropping) square
        int bankBalance(); // Return bank balance
        void addToBank(int coins); // Add an amount of coins to the bank
        void resetBank(); // Reset bank balance to 0
        bool checkVortexImpact(int x, int y); // Check if a vortex has hit any impactable actors at a specified location
        Player* winner(); // Determine the winner of the game

    private:
        Board m_board;
        std::vector<Actor*> m_actors;
        Player* m_peach;
        Player* m_yoshi;
        int m_bank;
};

#endif // STUDENTWORLD_H_
