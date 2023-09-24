#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <string>
using namespace std;

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath)
{
    m_board = Board();
    m_bank = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

// Access the board
Board StudentWorld::board()
{
    return m_board;
}

// Initialize the game
int StudentWorld::init()
{
    // Load the board
    string board_file = assetPath()+"board";
    board_file += "0"+to_string(getBoardNumber())+".txt";
            
    Board::LoadResult result = m_board.loadBoard(board_file);
    
    // Initialize all actors
    if (result == Board::load_fail_bad_format || result == Board::load_fail_file_not_found)
        return GWSTATUS_BOARD_ERROR;
            
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            for (int j = 0; j < BOARD_HEIGHT; j++)
            {
                switch (m_board.getContentsOf(i, j))
                {
                    case Board::player:
                    {
                        m_peach = new Player(1, this, IID_PEACH, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                        m_yoshi = new Player(2, this, IID_YOSHI, SPRITE_WIDTH*i, SPRITE_HEIGHT*j);
                        newActor(new CoinSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, BLUE));
                        break;
                    }
                    case Board::blue_coin_square:
                    {
                        newActor(new CoinSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, BLUE));
                        break;
                    }
                    case Board::red_coin_square:
                    {
                        newActor(new CoinSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, RED));
                        break;
                    }
                    case Board::up_dir_square:
                    {
                        newActor(new DirectionalSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, GraphObject::up));
                        break;
                    }
                    case Board::down_dir_square:
                    {
                        newActor(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, GraphObject::down));
                        break;
                    }
                    case Board::left_dir_square:
                    {
                        newActor(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, GraphObject::left));
                        break;
                    }
                    case Board::right_dir_square:
                    {
                        newActor(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j, GraphObject::right));
                        break;
                    }
                    case Board::event_square:
                    {
                        newActor(new EventSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j));
                        break;
                    }
                    case Board::bank_square:
                    {
                        newActor(new BankSquare(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j));
                        break;
                    }
                    case Board::star_square:
                    {
                        newActor(new StarSquare(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j));
                        break;
                    }
                    case Board::bowser:
                    {
                        newActor(new Bowser(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j));
                        newActor(new CoinSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, BLUE));
                        break;
                    }
                    case Board::boo:
                    {
                        newActor(new Boo(this, SPRITE_WIDTH * i, SPRITE_HEIGHT * j));
                        newActor(new CoinSquare(this, SPRITE_WIDTH*i, SPRITE_HEIGHT*j, BLUE));
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    
    // Start countdown and continue the game
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

// Update the board
int StudentWorld::move()
{
    // Set game stat text
    ostringstream oss;
    oss.setf(ios::fixed);
    
    oss << "P1 Roll: " << m_peach->getTicksToMove()/8 << " " << "Stars: " << m_peach->getStars() << " " << "$$: " << m_peach->getCoins() << " ";
    if (m_peach->hasVortex()) oss << "VOR ";
    oss << "| ";
    oss << "Time: " << timeRemaining() << " ";
    oss << "| ";
    oss << "Bank: " << m_bank << " " << "| ";
    oss << "P2 Roll: " << m_yoshi->getTicksToMove()/8 << " " << "Stars: " << m_yoshi->getStars() << " " << "$$: " << m_yoshi->getCoins();
    if (m_yoshi->hasVortex()) oss << " VOR";

    setGameStatText(oss.str());
    
    // Have all active actors do something, and delete all inactive ones
    if (m_peach) m_peach->doSomething();
    if (m_yoshi) m_yoshi->doSomething();
    
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); )
    {
        if (*it != nullptr)
        {
            if (!(*it)->isActive())
            {
                Actor* a = *it;
                it = m_actors.erase(it);
                delete a;
            }
            else
            {
                ((Actor*)*it)->doSomething();
                it++;
            }
        }
    }
    
    // Update final score
    setFinalScore(winner()->getStars(), winner()->getCoins());
    
    // End game
    if (timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
        if (winner() == m_peach) return  GWSTATUS_PEACH_WON;
        else return GWSTATUS_YOSHI_WON;
    }
    
    return GWSTATUS_CONTINUE_GAME; // If game is not over, continue
}

// Delete all actors
void StudentWorld::cleanUp()
{
    delete m_peach;
    m_peach = nullptr;
    delete m_yoshi;
    m_yoshi = nullptr;
    
    while (m_actors.size() > 0)
    {
        vector<Actor*>::iterator it = m_actors.begin();
        delete *it;
        m_actors.erase(it);
    }
}

// Allocate a new non-player actor
void StudentWorld::newActor(Actor* a) { m_actors.push_back(a); }

// Delete an actor during the game
void StudentWorld::removeActor(Actor* a)
{
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (*it == a)
        {
            it = m_actors.erase(it);
            delete *it;
            return;
        }
    }
}

Player* StudentWorld::player(int playerNum) { return playerNum == 1 ? m_peach : m_yoshi; } // Return a pointer to a player

// Delete a square at the current position of a new (dropping) square
void StudentWorld::replaceSquare(Actor* agent, Actor* newSquare)
{
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (!(*it)->isMovable() && (*it) != agent && agent->align(*it))
        {
            Actor* temp = *it;
            *it = newSquare;
            delete temp;
        }
    }
}

int StudentWorld::bankBalance() { return m_bank; } // Return bank balance

// Add an amount of coins to the bank
void StudentWorld::addToBank(int coins)
{
    m_bank += coins;
}

void StudentWorld::resetBank() {m_bank = 0;} // Reset bank balance to 0

// Check if a vortex has hit any impactable actors at a specified location
bool StudentWorld::checkVortexImpact(int x, int y)
{
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->impactable() && (*it)->vortexImpact(x, y))
            return true;
    }
    return false;
}

// Determine the winner of the game
Player* StudentWorld::winner()
{
    Player* winner = m_peach;
    
    if (m_peach->getStars() > m_yoshi->getStars()) winner = m_peach;
    else if (m_peach->getStars() < m_yoshi->getStars()) winner = m_yoshi;
    
    if (m_peach->getStars() == m_yoshi->getStars())
    {
        if (m_peach->getCoins() > m_yoshi->getCoins()) winner = m_peach;
        else if (m_peach->getCoins() < m_yoshi->getCoins()) winner = m_yoshi;
        else
        {
            switch (randInt(0,1))
            {
                case 0:
                    winner = m_peach;
                    break;
                case 1:
                    winner = m_yoshi;
                    break;
            }
        }
    }
    
    return winner;
}
