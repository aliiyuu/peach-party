#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath)
{
    m_board = Board();
    
    // Starting coordinates
}

// Destructor
StudentWorld::~StudentWorld()
{
// Somehow this doesn't work when I put it in cleanUp()
}

Board StudentWorld::board()
{
    return m_board;
}

int StudentWorld::init()
{
    //startCountdownTimer(5);  // this placeholder causes timeout after 5 seconds
    //return GWSTATUS_CONTINUE_GAME;
    string board_file = assetPath()+"board";
    board_file += "0"+to_string(getBoardNumber())+".txt";
            
    Board::LoadResult result = m_board.loadBoard(board_file);
    
    if (result == Board::load_fail_bad_format || result == Board::load_fail_file_not_found)
        return GWSTATUS_BOARD_ERROR;
            
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            for (int j = 0; j < BOARD_HEIGHT; j++)
            {
                switch (m_board.getContentsOf(i, j))
                {
                    case Board::empty: {
                        break;
                    }
                    case Board::player:
                    {
                        m_peach = new Player(1, this, IID_PEACH, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                        m_yoshi = new Player(2, this, IID_YOSHI, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                        m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::blue_coin_square:
                    {
                        m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::red_coin_square: {
                        m_actors.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::up_dir_square: {
                        m_actors.push_back(new CoinSquare(this, IID_DIR_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::down_dir_square: {
                        m_actors.push_back(new CoinSquare(this, IID_DIR_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::left_dir_square: {
                        m_actors.push_back(new CoinSquare(this, IID_DIR_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::right_dir_square: {
                        m_actors.push_back(new CoinSquare(this, IID_DIR_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::event_square: {
                        m_actors.push_back(new CoinSquare(this, IID_EVENT_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::bank_square: {
                        m_actors.push_back(new CoinSquare(this, IID_BANK_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::star_square: {
                        m_actors.push_back(new CoinSquare(this, IID_STAR_SQUARE, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::bowser: {
                        m_actors.push_back(new CoinSquare(this, IID_BOWSER, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::boo: {
                        m_actors.push_back(new CoinSquare(this, IID_BOO, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                }
            }
        }
    
    startCountdownTimer(100);

    
    return GWSTATUS_CONTINUE_GAME;
}

Player* StudentWorld::peach()
{
    return m_peach;
}

Player* StudentWorld::yoshi()
{
    return m_yoshi;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.
    
    ostringstream oss;
    oss.setf(ios::fixed);
    oss << "P1 Roll: ";
    oss << m_peach->getTicksToMove()/8 << " ";
    oss << "Stars: ";
    oss << m_peach->getStars() << " ";
    oss << "$$: ";
    oss << m_peach->getCoins() << " ";
    oss << "| ";
    oss << "Time: ";
    oss << timeRemaining() << " ";
    oss << "| ";
    oss << "Bank: ";
    oss << 0 << " ";
    oss << "| ";
    oss << "P2 Roll: ";
    oss << m_yoshi->getTicksToMove()/8 << " ";
    oss << "Stars: ";
    oss << m_yoshi->getStars() << " ";
    oss << "$$: ";
    oss << m_yoshi->getCoins();

    string output = oss.str();
    

    setGameStatText(output);
    
    m_peach->doSomething();
    m_yoshi->doSomething();
    
    for (int i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->isActive())
        {
            m_actors[i]->doSomething();
        }

    if (timeRemaining() <= 0)
        return  GWSTATUS_PEACH_WON; // placeholder
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    while (m_actors.size() > 0)
    {
        vector<Actor*>::iterator it = m_actors.begin();
        delete *it;
        m_actors.erase(it);
    }
    delete m_peach;
    delete m_yoshi;
}

// get an actor (to check overlap)
Actor* StudentWorld::getActor(int i)
{
    if (i < m_actors.size() && i >= 0)
        return m_actors[i];
    return nullptr;
}


