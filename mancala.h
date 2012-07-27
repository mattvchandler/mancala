//mancala.h
//Mancala AI
//Copyright Matthew Chandler 2012
#ifndef __MANCALA_H__
#define __MANCALA_H__
#include <vector>

struct Bowl
{
public:
    Bowl(const int Count = 0, const int Next = 0, const int Across = 0):
        count(Count), next(Next), across(Across)
    {}

    int count;
    int next, across;
};

class Board
{
public:
    Board(const int Num_bowls = 6, const int Num_seeds = 4);
    //perform a move
    //returns true if the move earns an extra turn
    bool move(int bowl);

    //swap sides of the board
    void swapsides();

    //is the game over
    bool finished() const;

    //heuristics to evaluate the board status
    int evaluate() const;

    void crapprint() const; //delete me!

    int num_bowls, num_seeds;
    std::vector<Bowl> bowls;
    int p1_start, p2_start;
    int p1_store, p2_store;
};

int choosemove(Board b); //purposely doing pass by value here as to not corrupt passed board
#endif
