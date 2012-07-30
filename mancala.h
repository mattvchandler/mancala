// mancala.h
// Mancala AI
// Copyright Matthew Chandler 2012
#ifndef __MANCALA_H__
#define __MANCALA_H__
#include <vector>

enum PLAYER {PLAYER_MIN, PLAYER_MAX};

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
    // perform a move
    // returns true if the move earns an extra turn
    bool move(int bowl);

    // swap sides of the board
    void swapsides();

    // is the game over
    bool finished() const;

    // heuristics to evaluate the board status
    int evaluate() const;

    void crapprint() const; // delete me!

    int num_bowls, num_seeds;
    
    friend int choosemove(const Board b);
    friend int choosemove_alphabeta(const Board b, int depth, PLAYER player, int alpha, int beta);
    friend void cli_game();
    friend class Mancala_win;
private:
    std::vector<Bowl> bowls;
    int p1_start, p2_start;
    int p1_store, p2_store;
};

int choosemove(const Board b); // purposely doing pass by value here as to not corrupt passed board
#endif
