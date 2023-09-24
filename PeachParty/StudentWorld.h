#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include <list>
#include "Board.h"
#include <string>
class Actor;
class Player;
class CoinSquare;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
    ~StudentWorld();
    Board board();
    Player* peach();
    Player* yoshi();
    CoinSquare* getCoinSquare(int i);
    Actor* getActor(int i);

  virtual int init();
  virtual int move();
  virtual void cleanUp();

private:
    Board m_board;
   // Player* m_p1;
    //Player* m_p2;
    std::vector<Actor*> m_actors; //pointer issues gang
    Player* m_peach; // not sure if can have player pointers
    Player* m_yoshi;
};

#endif // STUDENTWORLD_H_
