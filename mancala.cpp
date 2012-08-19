// mancala.cpp
// Mancala AI
// Copyright Matthew Chandler 2012

#include <vector>
#include <limits>
#include <cstdlib>

#include "mancala.h"

// board:
// 2 rows of 6 bowls
// 2 larger bowls for scoring (stores)
//
// Rules
// play is CCW
// ending in player's own store yields extra turn
// ending in empty bowl earns that piece and all those in the bowl across from it
// game ends when a player has no legal moves left
//
// good heuristics:
// score - opponent's score
// possibly good:
// number of availible moves
// seeds in play
// seed distribution (large piles waiting to be collected? seed ratio between sides)
// possibilty of extra turns
// if we wanted to, we could use a genetic algorithm for determining the importance of each of these
//
// representation
// circular array?
//  11 10 9  8  7  6
//  0  1  2  3  4  5
//  p1 store, p2 store
// From there, we would need logic to add to player's own store and skip the opponents.
// if we abstract out the actual array indexes, we can use 2 starting pointers to easily flip the board
// With that in mind, it might be better to use a Digraph, so we can have a truly circular setup
// each node could have a next, prev, and across pointer, and one that points to the stores for the ends (NULL otherwise)
//
//
//
// If we wanted to get really fancy, we could do 3D graphics with particle physics for the seeds.

Board::Board(const int Num_bowls, const int Num_seeds):
num_bowls(Num_bowls), num_seeds(Num_seeds)
{
    bowls.resize(2 * num_bowls + 2);
    p1_start = 0;
    p1_store = num_bowls;
    p2_start = num_bowls + 1;
    p2_store = 2 * num_bowls + 1;

    for(size_t i = 0; i < bowls.size(); ++i)
    {
        if(i < bowls.size() - 1)
            bowls[i].next = i + 1;
        else
            bowls[i].next = 0;

        if(i != (size_t)num_bowls && i != 2 * (size_t)num_bowls + 1)
        {
            bowls[i].across = 2 * num_bowls - i;
            bowls[i].count = num_seeds;
        }
    }
}

// perform a move
// returns true if the move earns an extra turn
bool Board::move(int bowl)
{
    if(bowl < 0 || bowl >= num_bowls)
        return false;
    bowl += p1_start;
    int seeds = bowls[bowl].count;
    if(seeds == 0)
        return false;
    bowls[bowl].count = 0;
    // make the move
    for(int i = 0; i < seeds; ++i)
    {
        bowl = bowls[bowl].next;
        if(bowl == p2_store)
            bowl = bowls[bowl].next;
        bowls[bowl].count += 1;
    }
    // extra turn if we land in our own store
    if(bowl == p1_store)
        return true;

    // if we land in an empty bowl, we get the last seed sown and all the seeds from the bowl across
    if(bowls[bowl].count == 1 && bowls[bowls[bowl].across].count > 0)
    {
        bowls[p1_store].count += 1 + bowls[bowls[bowl].across].count;
        bowls[bowls[bowl].across].count = 0;
        bowls[bowl].count = 0;
    }
    return false;
}

// swap sides of the board
void Board::swapsides()
{
    std::swap(p1_start, p2_start);
    std::swap(p1_store, p2_store);
}

// is the game over
bool Board::finished() const
{
    int p1_side = 0;
    for(int i = p1_start; i < p1_start + num_bowls; ++i)
        p1_side |= bowls[i].count;
    if(p1_side == 0)
        return true;
    int p2_side = 0;
    for(int i = p2_start; i < p2_start + num_bowls; ++i)
        p2_side |= bowls[i].count;
    return p2_side == 0;
}

// heuristics to evaluate the board status
int Board::evaluate() const
{
    // simple - will probably need improvement
    return bowls[p1_store].count - bowls[p2_store].count;
}

int choosemove_alphabeta(const Board b, int depth, PLAYER player, int alpha, int beta)
{
    if(player == PLAYER_MAX)
    {
        if(depth == 0)
            return b.evaluate();
        // move toward closest win, avoid loss as long as possible
        if(b.finished())
        {
            int diff = b.evaluate();
            if(diff == 0)
                return depth;
            else if(diff > 0)
                return 1000 + diff + depth;
            else
                return -1000 + diff - depth;
        }
        for(int i = 0; i < b.num_bowls; ++i)
        {
            if(b.bowls[b.p1_start + i].count == 0)
                continue;
            Board sub_b = b;
            int score = 0;
            if(sub_b.move(i)) // do we get another move?
                score = choosemove_alphabeta(sub_b, depth - 1, PLAYER_MAX, alpha, beta);
            else
            {
                sub_b.swapsides();
                score = choosemove_alphabeta(sub_b, depth - 1, PLAYER_MIN, alpha, beta);
            }
            if(score >= beta)
                return beta;
            if(score > alpha)
                alpha = score;
        }
        return alpha;
    }
    else
    {
        if(depth == 0)
            return -b.evaluate();
        // move toward closest win, avoid loss as long as possible
        if(b.finished())
        {
            int diff = b.evaluate();
            if(diff == 0)
                return -depth;
            else if(diff > 0)
                return -1000 - diff - depth;
            else
                return 1000 - diff + depth;
        }
        for(int i = 0; i < b.num_bowls; ++i)
        {
            if(b.bowls[b.p1_start + i].count == 0)
                continue;
            Board sub_b = b;
            int score = 0;
            if(sub_b.move(i)) // do we get another move?
                score = choosemove_alphabeta(sub_b, depth - 1, PLAYER_MIN, alpha, beta);
            else
            {
                sub_b.swapsides();
                score = choosemove_alphabeta(sub_b, depth - 1, PLAYER_MAX, alpha, beta);
            }
            if(score <= alpha)
                return alpha;
            if(score < beta)
                beta = score;
        }
        return beta;
    }
}

int choosemove(const Board b) // purposely doing pass by value here as to not corrupt passed board
{
    if(b.finished())
        return 0;
    int best = std::numeric_limits<int>::min();
    std::vector<int> best_i;
    // loop over available moves
    for(int i =0; i < b.num_bowls; ++i)
    {
        if(b.bowls[b.p1_start + i].count == 0)
            continue;
        Board sub_b = b;
        int score = 0;
        if(sub_b.move(i))
            score = choosemove_alphabeta(sub_b, 10, PLAYER_MAX, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        else
        {
            sub_b.swapsides();
            score = choosemove_alphabeta(sub_b, 10, PLAYER_MIN, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        }
        // std::cout<<"choose: "<<i<<" "<<score<<" "<<-sub_b.evaluate()<<std::endl;
        if(score > best)
        {
            best = score;
            best_i.clear();
            best_i.push_back(i);
        }
        if(score == best);
            best_i.push_back(i);
    }
    return best_i[rand() % best_i.size()];
}
