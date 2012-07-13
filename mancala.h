//Mancala AI
//Copyright Matthew Chandler 2012
#ifndef __MANCALA_H__
#define __MANCALA_H__
#include <vector>
//board:
//2 rows of 6 bowls
//2 larger bowls for scoring (stores)
//
//Rules
//play is CCW
//ending in player's own store yields extra turn
//ending in empty bowl earns that piece and all those in the bowl across from it
//game ends when a player has no legal moves left
//
//good heuristics:
//score - opponent's score
//possibly good:
//number of availible moves
//seeds in play
//seed distribution (large piles waiting to be collected? seed ratio between sides)
//possibilty of extra turns
//if we wanted to, we could use a genetic algorithm for determining the importance of each of these
//
//representation
//circular array?
//  11 10 9  8  7  6
//  0  1  2  3  4  5
//  p1 store, p2 store
//From there, we would need logic to add to player's own store and skip the opponents.
//if we abstract out the actual array indexes, we can use 2 starting pointers to easily flip the board
//With that in mind, it might be better to use a Digraph, so we can have a truly circular setup
//each node could have a next, prev, and across pointer, and one that points to the stores for the ends (NULL otherwise)
//
//
//
//If we wanted to get really fancy, we could do 3D graphics with particle physics for the seeds.

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
