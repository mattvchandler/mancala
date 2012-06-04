//Mancala AI
//Copyright Matthew Chandler 2012

#include <iostream>
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
    Board(const int Num_bowls = 6, const int Num_seeds = 4):
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

            if(i != num_bowls && i != 2 * num_bowls + 1)
            {
                bowls[i].across = 2 * num_bowls - i;
                bowls[i].count = num_seeds;
            }
        }
    }
    //perform a move
    //returns true if the move earns an extra turn
    bool move(int bowl)
    {
        if(bowl < 0 || bowl >= num_bowls)
            return false;
        bowl += p1_start;
        int seeds = bowls[bowl].count;
        if(seeds == 0)
            return false;
        bowls[bowl].count = 0;
        //make the move
        for(int i = 0; i < seeds; ++i)
        {
            bowl = bowls[bowl].next;
            if(bowl == p2_store)
                bowl = bowls[bowl].next;
            bowls[bowl].count += 1;
        }
        //extra turn if we land in our own store
        if(bowl == p1_store)
            return true;

        //if we land in an empty bowl, we get the last seed sown and all the seeds from the bowl across
        if(bowls[bowl].count == 1 && bowls[bowls[bowl].across].count > 0)
        {
            bowls[p1_store].count += 1 + bowls[bowls[bowl].across].count;
            bowls[bowls[bowl].across].count = 0;
            bowls[bowl].count = 0;
        }
            

        return false;
    }

    //swap sides of the board
    void swapsides()
    {
        std::swap(p1_start, p2_start);
        std::swap(p1_store, p2_store);
    }

    void crapprint() const //delete me!
    {
        std::cout<<bowls[p2_store].count<<std::endl;
        for(int i = p1_start; i < p1_start + 6; ++i)
            std::cout<<bowls[i].count<<"\t"<<bowls[bowls[i].across].count<<std::endl;
        std::cout<<bowls[p1_store].count<<std::endl;
    }

    int num_bowls, num_seeds;
    std::vector<Bowl> bowls;
    int p1_start, p2_start;
    int p1_store, p2_store;
};

int main()
{
    Board b;
    return 0;
}
