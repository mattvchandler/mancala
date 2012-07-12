//Mancala AI
//Copyright Matthew Chandler 2012

#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <cstdlib>

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

            if(i != (size_t)num_bowls && i != 2 * (size_t)num_bowls + 1)
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

    //is the game over
    bool finished() const
    {
        int p1_side = 0;
        for(int i = p1_start; i < p1_start + 6; ++i)
            p1_side |= bowls[i].count;
        if(p1_side == 0)
            return true;
        int p2_side = 0;
        for(int i = p2_start; i < p2_start + 6; ++i)
            p2_side |= bowls[i].count;
        return p2_side == 0;
    }

    //heuristics to evaluate the board status
    int evaluate() const
    {
        //simple
        return bowls[p1_store].count - bowls[p2_store].count;
    }

    void crapprint() const //delete me!
    {
        std::cout<<"    ";
        for(int i = p1_start; i < p1_start + 6; ++i)
            std::cout<<std::setw(2)<<std::setfill(' ')<<bowls[bowls[i].across].count<<" ";
        std::cout<<std::endl;
        std::cout<<std::setw(2)<<std::setfill(' ')<<bowls[p2_store].count<<std::setw(3*7)<<" "<<std::setw(2)<<bowls[p1_store].count<<std::endl;
        std::cout<<"    ";
        for(int i = p1_start; i < p1_start + 6; ++i)
            std::cout<<std::setw(2)<<std::setfill(' ')<<bowls[i].count<<" ";
        std::cout<<std::endl;
        std::cout<<"    ";
        for(int i = 0; i < 6 ; ++i)
            std::cout<<std::setw(2)<<std::setfill(' ')<<i<<" ";
        std::cout<<std::endl;
    }

    int num_bowls, num_seeds;
    std::vector<Bowl> bowls;
    int p1_start, p2_start;
    int p1_store, p2_store;
};

enum PLAYER {PLAYER_MIN, PLAYER_MAX};

int choosemove_minimax(Board b, int depth, PLAYER player)
{
    if(player == PLAYER_MAX)
    {
        if(depth == 0)
            return b.evaluate();
        if(b.finished())
        {
            int diff = b.evaluate();
            if(diff > 0)
                return 1000 + depth;
            else
                return -1000 - depth;
        }
        int max = std::numeric_limits<int>::min();
        for(int i = 0; i < 6; ++i)
        {
            if(b.bowls[b.p1_start + i].count == 0)
                continue;
            Board sub_b = b;
            sub_b.move(i); // do we get another move?
            sub_b.swapsides();
            //max = std::max(max, choosemove_minimax(sub_b, depth - 1, PLAYER_MIN));
            //sub_b.crapprint();
            //for(int i = 3-depth; i >=0; --i)
            //    std::cout<<"  ";
            //std::cout<<"i: "<<i<<std::endl;
            int score = choosemove_minimax(sub_b, depth - 1, PLAYER_MIN);
            max = std::max(max,score);
            //for(int i = 3-depth; i >=0; --i)
            //    std::cout<<"  ";
            //std::cout<<"score: "<<score<<std::endl;
        }
        return max;
    }
    else
    {
        if(depth == 0)
            return -b.evaluate();
        if(b.finished())
        {
            int diff = b.evaluate();
            if(diff > 0)
                return -1000 - depth;
            else
                return 1000 + depth;
        }
        int min = std::numeric_limits<int>::max();
        for(int i = 0; i < 6; ++i)
        {
            if(b.bowls[b.p1_start + i].count == 0)
                continue;
            Board sub_b = b;
            sub_b.move(i); // do we get another move?
            sub_b.swapsides();
            //min = std::min(min, choosemove_minimax(sub_b, depth - 1, PLAYER_MAX));
            //sub_b.crapprint();
            //for(int i = 3-depth; i >=0; --i)
            //    std::cout<<"  ";
            //std::cout<<"i: "<<i<<std::endl;
            int score = choosemove_minimax(sub_b, depth - 1, PLAYER_MAX);
            min = std::min(min, score);
            //for(int i = 3-depth; i >=0; --i)
            //    std::cout<<"  ";
            //std::cout<<"score: "<<score<<std::endl;
        }
        return min;
    }
}

int choosemove(Board b) //purposely doing pass by value here as to not corrupt passed board (we may want to use different method when we do actual move search)
{
    int best = std::numeric_limits<int>::min();
    std::vector<int> best_i;
    //loop over available moves
    for(int i =0; i < 6; ++i)
    {
        if(b.bowls[b.p1_start + i].count == 0)
            continue;
        Board sub_b = b;
        sub_b.move(i);
        sub_b.swapsides();
        int score = choosemove_minimax(sub_b, 10, PLAYER_MIN);
        std::cout<<"choose: "<<i<<" "<<score<<" "<<-sub_b.evaluate()<<std::endl;
        if(score > best)
        {
            best = score;
            best_i.clear();
            best_i.push_back(i);
        }
        if(score == best);
            best_i.push_back(i);
    }
    return best_i[0];//rand() % best_i.size()];
}

int main()
{
    srand(time(0));
    Board b;
    //b.bowls=std::vector<Bowl>({Bowl(3,1,12), Bowl(2,2,11), Bowl(1,3,10), Bowl(0,4,9), Bowl(10,5,8), Bowl(0,6,7), Bowl(0,7,0), Bowl(1,8,5), Bowl(0,9,4), Bowl(10,10,3), Bowl(1,11,2), Bowl(1,12,1), Bowl(1,13,0), Bowl(0,0,0)});
    char nextmove = '\0';
    int player = 1;
    while(!b.finished() && nextmove != 'q' && nextmove != 'Q')
    {
        std::cout<<"Player "<<player<<std::endl;
        b.crapprint();
        std::cout<<"Best move: "<<choosemove(b)<<std::endl;
        std::cout<<"Next move: ";
        std::cin>>nextmove;
        std::cout<<std::endl;
        if(nextmove == 'S' || nextmove == 's')
        {
            b.swapsides();
            player = (player == 1) ? 2 : 1;
        }
        if(nextmove >= '0' && nextmove <= '5')
        {
            if(!b.move(nextmove - '0'));
            {
                b.swapsides();
                player = (player == 1) ? 2 : 1;
            }
        }
    }
    if(b.finished())
    {
        std::cout<<"Player "<<player<<std::endl;
        b.crapprint();
        if(b.bowls[b.p1_store].count == b.bowls[b.p2_store].count)
            std::cout<<"Tie"<<std::endl;
        else if(player == 1)
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                std::cout<<"Player 1 wins"<<std::endl;
            else
                std::cout<<"Player 2 wins"<<std::endl;
        else
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                std::cout<<"Player 2 wins"<<std::endl;
            else
                std::cout<<"Player 1 wins"<<std::endl;
        if(abs(b.bowls[b.p1_store].count - b.bowls[b.p2_store].count) >= 10)
            std::cout<<"FATALITY"<<std::endl;
    }
    return 0;
}
