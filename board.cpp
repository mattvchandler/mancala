// board.cpp
// Mancala board representation
// Copyright Matthew Chandler 2012

#ifdef DEBUG
#include <iostream>
#endif
#include <limits>
#include <cmath>
#include <cstdlib>

#include "board.h"

// random double between 0 and 1
double randd()
{
    float f = rand();
    return f / RAND_MAX;
}

// a random vector from a given point constrained by width and height
std::vector<double> rand_pos(const std::vector<double> & ul, double width, double height)
{
    auto pos = ul;
    double theta = randd() * 2 * M_PI;
    double r = randd();
    pos[0] += cos(theta) * r * width * .20;
    pos[1] += sin(theta) * r * height * .20;
    return pos;
}

// simple bowl and board classes to speed up ai search
struct Simple_bowl
{
    int count;
    // indexes to other bowls.
    // using indexes instead of pointers to allow fast copying
    int next_i;
    int across_i;
};

class Simple_board
{
public:
    Simple_board(const Mancala::Board & b);

    // perform a move
    // returns true if the move earns an extra turn
    bool move(const Mancala::Player p, const int i);
    // is the game over
    bool finished() const;
    // heuristics to evaluate the board status
    int evaluate() const;

#ifdef DEBUG
    // board print function for debugging AI
    void debug_print() const;
#endif

    // board layout: <bottom row><r_store><top_row><l_store>
    int num_bowls;
    std::vector<Simple_bowl> bowls;
    bool extra_rule, capture_rule, collect_rule;
};

Simple_board::Simple_board(const Mancala::Board & b): num_bowls(b.num_bowls), bowls(2 * b.num_bowls + 2),
    extra_rule(b.extra_rule), capture_rule(b.capture_rule), collect_rule(b.collect_rule)
{
    // copy bead counts from Board obj, set indexes
    for(int i = 0; i < num_bowls; ++i)
    {
        bowls[i].count = b.bottom_row[i].beads.size();
        bowls[2 * num_bowls - i].count = b.top_row[i].beads.size();

        bowls[i].across_i = 2 * num_bowls - i;
        bowls[2 * num_bowls - i].across_i = i;
    }

    bowls[num_bowls].count = b.r_store.beads.size();
    bowls[2 * num_bowls + 1].count = b.r_store.beads.size();

    for(size_t i = 0; i < bowls.size() - 1; ++i)
        bowls[i].next_i = i + 1;

    bowls[2 * num_bowls + 1].next_i = 0;
}

// perform a move
// returns true if the move earns an extra turn
bool Simple_board::move(const Mancala::Player p, const int i)
{
    bool extra_move = false;
    // get important indexes
    int curr = (p == Mancala::PLAYER_1)? i: 2 * num_bowls - i;
    int store = (p == Mancala::PLAYER_1)? num_bowls: 2 * num_bowls + 1;
    int wrong_store = (p == Mancala::PLAYER_1)? 2 * num_bowls + 1: num_bowls;

    // take beads from start and put into hand
    int hand = bowls[curr].count;
    bowls[curr].count = 0;

    // place each bead from the starting bowl
    while(hand > 0)
    {
        curr = bowls[curr].next_i;
        // skip opponent's store
        if(curr == wrong_store)
            curr = bowls[curr].next_i;
        ++bowls[curr].count;
        --hand;
    }

    // extra move when ending in our store
    if(extra_rule && curr == store)
        extra_move = true;

    else if(capture_rule)
    {
        // collect last bead, and all beads across from it if we land in an empty bowl
        if(bowls[curr].count == 1 && bowls[bowls[curr].across_i].count > 0)
        {
            bowls[store].count += bowls[bowls[curr].across_i].count + 1;
            bowls[curr].count = 0;
            bowls[bowls[curr].across_i].count = 0;
        }
    }

    // when one side is empty, move all beads on the other side to that player's store
    if(collect_rule)
    {
        int p1 = 0, p2 = 0;
        for(int i = 0; i < num_bowls; ++i)
        {
            p1 += bowls[i].count;
            p2 += bowls[num_bowls + 1 + i].count;
        }

        if(p1 == 0 && p2 != 0)
        {
            for(int i = num_bowls + 1; i < 2 * num_bowls + 1; ++i)
            {
                bowls[2 * num_bowls + 1].count += bowls[i].count;
                bowls[i].count = 0;
            }
        }
        else if(p2 == 0 && p1 !=0)
        {
            for(int i = 0; i < num_bowls; ++i)
            {
                bowls[num_bowls].count += bowls[i].count;
                bowls[i].count = 0;
            }
        }
    }

    return extra_move;
}

// is the game over
bool Simple_board::finished() const
{
    int p1 = 0, p2 = 0;
    for(int i = 0; i < num_bowls; ++i)
    {
        p1 += bowls[i].count;
        p2 += bowls[num_bowls + 1 + i].count;
    }
    return p1 == 0 || p2 == 0;
}

#ifdef DEBUG
// board print function for debugging AI
void Simple_board::debug_print() const
{
    for(int i = 0; i < num_bowls; ++i)
        std::cout<<bowls[2 * num_bowls - i].count<<" ";
    std::cout<<std::endl;
    for(int i = 0; i < num_bowls; ++i)
        std::cout<<bowls[i].count<<" ";
    std::cout<<std::endl<<bowls[num_bowls* 2 + 1].count<<" | "<<bowls[num_bowls].count<<std::endl;
}
#endif

// heuristics to evaluate the board status
// only needs to evaluate for p1
int Simple_board::evaluate() const
{
    // assume that p2 gets all of the remaining beads at the end
    int board_count = 0;
    if(collect_rule)
        for(int i = 0; i < num_bowls; ++i)
            board_count += bowls[i].count + bowls[i + num_bowls + 1].count;

    return bowls[num_bowls].count - bowls.back().count - board_count;
}

// helper recursive function for choosemove
// we evaluate for player 1, so take the negative of the score when called for p2
int choosemove_alphabeta(const Simple_board & b, const int depth, const Mancala::Player p,
    int alpha, int beta)
{
#ifdef DEBUG
    b.debug_print();
    std::cout<<std::endl;
#endif
    if(p == Mancala::PLAYER_1)
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
        // recursively try each possible move
        for(int i = 0; i < b.num_bowls; ++i)
        {
            if(b.bowls[i].count == 0)
                continue;
#ifdef DEBUG
            std::cout<<"p1 move "<<i<<" depth "<<depth<<std::endl;
#endif
            Simple_board sub_b = b;
            int score = 0;
            if(sub_b.move(Mancala::PLAYER_1, i)) // do we get another move?
                score = choosemove_alphabeta(sub_b, depth - 1, Mancala::PLAYER_1, alpha, beta);
            else
                score = choosemove_alphabeta(sub_b, depth - 1, Mancala::PLAYER_2, alpha, beta);
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
        // recursively try each possible move
        for(int i = 0; i < b.num_bowls; ++i)
        {
            if(b.bowls[2 * b.num_bowls - i].count == 0)
                continue;
#ifdef DEBUG
            std::cout<<"p2 move "<<i<<" depth "<<depth<<std::endl;
#endif
            Simple_board sub_b = b;
            int score = 0;
            if(sub_b.move(Mancala::PLAYER_2, i)) // do we get another move?
                score = choosemove_alphabeta(sub_b, depth - 1, Mancala::PLAYER_2, alpha, beta);
            else
                score = choosemove_alphabeta(sub_b, depth - 1, Mancala::PLAYER_1, alpha, beta);
            if(score <= alpha)
                return alpha;
            if(score < beta)
                beta = score;
        }
        return beta;
    }
}

void choosemove_thread_func(const Mancala::Board & b, const Mancala::Player p)
{
    int move = b.choosemove(p);
    // emit signal
    std::thread::id id = std::this_thread::get_id();
    b.signal_choosemove_sig.emit(move, id);
}

namespace Mancala
{
    const int NUM_COLORS = 6;

    Bead::Bead(const std::vector<double> & Pos, const int Color_i):
        pos(Pos), color_i(Color_i)
    {}

    Bowl::Bowl(const int Count, const std::vector<double> & Ul,
        const double Width, const double Height):
        ul(Ul),
        width(Width),
        height(Height),
        beads(Count),
        next(NULL),
        across(NULL)
    {
        if(!beads.empty())
            redist_beads();
        for(auto &i: beads)
            i.color_i = rand() % Mancala::NUM_COLORS;
    }

    // add a new bead
    void Bowl::add_bead(const Mancala::Bead & new_bead)
    {
        beads.push_back(new_bead);

        // assign it a new position
        if(beads.size() <= 4)
            redist_beads();
        else
            beads.back().pos = rand_pos(ul, width, height);
    }

    // redistribute the beads
    void Bowl::redist_beads()
    {
        // center if only one bead
        if(beads.size() == 1)
        {
            beads[0].pos = ul;
            return;
        }

        // for 2-4, use jittered grid dist
        int num_dist_beads = std::min((int)beads.size(), 4);
        std::vector<std::vector<double>> grid_pos(num_dist_beads, std::vector<double>(4));

        switch(num_dist_beads)
        {
        case 2:
            grid_pos[0] = {ul[0], ul[1] - height * .1, height * .1, width};
            grid_pos[1] = {ul[0], ul[1] + height * .1, height * .1, width};
            break;
        case 3:
            grid_pos[0] = {ul[0] + width * .05, ul[1] - height * .1, height * .1, width * .1};
            grid_pos[1] = {ul[0] + width * .1, ul[1] + height * .1, height * .1, width * .1};
            grid_pos[2] = {ul[0] - width * .1, ul[1] + height * .1, height * .1, width * .1};
            break;
        case 4:
        default:
            grid_pos[0] = {ul[0] - width * .1, ul[1] - height * .1, height * .1, width * .1};
            grid_pos[1] = {ul[0] + width * .1, ul[1] - height * .1, height * .1, width * .1};
            grid_pos[2] = {ul[0] + width * .1, ul[1] + height * .1, height * .1, width * .1};
            grid_pos[3] = {ul[0] - width * .1, ul[1] + height * .1, height * .1, width * .1};
            break;
        }

        for(int i = 0; i < num_dist_beads; ++i)
                beads[i].pos = rand_pos({grid_pos[i][0], grid_pos[i][1]}, grid_pos[i][2], grid_pos[i][3]);

        // for >4 use random dist
        if(beads.size() > 4)
            for(size_t i = 4; i < beads.size(); ++i)
                beads[i].pos = rand_pos(ul, width, height);
    }

    Board::Board(const int Num_bowls, const int Num_beads, const int Ai_depth,
        const bool Extra_rule, const bool Capture_rule, const bool Collect_rule):
        num_bowls(Num_bowls),
        num_beads(Num_beads),
        ai_depth(Ai_depth),
        extra_rule(Extra_rule),
        capture_rule(Capture_rule),
        collect_rule(Collect_rule)
    {
        set_bowls();
    }

    Board::Board(const Board & b):
        num_bowls(b.num_bowls),
        num_beads(b.num_beads),
        ai_depth(b.ai_depth),
        extra_rule(b.extra_rule),
        capture_rule(b.capture_rule),
        collect_rule(b.collect_rule),
        top_row(b.top_row),
        bottom_row(b.bottom_row),
        l_store(b.l_store),
        r_store(b.r_store)
    {
        set_bowls();
    }

    Board & Board::operator=(const Board & b)
    {
        if(this != &b)
        {
            num_bowls = b.num_bowls;
            num_beads = b.num_beads;
            ai_depth = b.ai_depth;
            extra_rule = b.extra_rule;
            capture_rule = b.capture_rule;
            collect_rule = b.collect_rule;
            top_row = b.top_row;
            bottom_row = b.bottom_row;
            l_store = b.l_store;
            r_store = b.r_store;

            set_bowls();
        }
        return *this;
    }

    // set up bowls
    void Board::set_bowls()
    {
        int num_cells = num_bowls + 2;
        double inv_num_cells = 1.0 / num_cells;

        // set stores as empty, and set up coords
        l_store = Bowl(0, {0.0, .25}, inv_num_cells, 1.0);
        r_store = Bowl(0, {1.0 - inv_num_cells, .25}, inv_num_cells, 1.0);

        top_row.clear();
        bottom_row.clear();

        // set rows with starting beads, set coords
        for(int i = 0; i < num_bowls; ++i)
        {
            top_row.emplace_back(Bowl(num_beads, {(double)(i + 1) * inv_num_cells, 0.0}, inv_num_cells, .5));
            bottom_row.emplace_back(Bowl(num_beads, {(double)(i + 1) * inv_num_cells, 0.5}, inv_num_cells, .5));
        }

        // set up bowl pointers
        l_store.next = &bottom_row.front();
        r_store.next = &top_row.back();

        for(size_t i = 0; i < bottom_row.size() - 1; ++i)
            bottom_row[i].next = &bottom_row[i + 1];

        for(size_t i = 1; i < top_row.size() ; ++i)
            top_row[i].next = &top_row[i - 1];

        top_row.front().next = &l_store;
        bottom_row.back().next = &r_store;

        for(size_t i = 0; i < top_row.size(); ++i)
        {
            bottom_row[i].across = &top_row[i];
            top_row[i].across = &bottom_row[i];
        }
    }

    // perform a move
    // returns true if the move earns an extra turn
    bool Board::move(const Mancala::Player p, const int i)
    {
        bool extra_move = false;
        // get important pointers
        Bowl * curr  = (p == PLAYER_1)? &bottom_row[i]: &top_row[i];
        Bowl * store = (p == PLAYER_1)? &r_store: &l_store;
        Bowl * wrong_store = (p == PLAYER_1)? &l_store: &r_store;

        // take beads from start and put into hand
        Bowl hand = *curr;
        curr->beads.clear();

        // place each bead from the starting bowl
        while(hand.beads.size() > 0)
        {
            curr = curr->next;
            // skip opponent's store
            if(curr == wrong_store)
                curr = curr->next;
            curr->add_bead(hand.beads.back());
            hand.beads.pop_back();
        }

        // extra move when ending in our store
        if(extra_rule && curr == store)
            extra_move = true;

        else if(capture_rule)
        {
            // collect last bead, and all beads across from it if we land in an empty bowl
            if(curr->beads.size() == 1 && curr->across->beads.size() > 0)
            {
                store->add_bead(curr->beads.front());
                curr->beads.clear();
                for(auto & i: curr->across->beads)
                    store->add_bead(i);
                curr->across->beads.clear();
            }
        }

        // when one side is empty, move all beads on the other side to that player's store
        if(collect_rule)
        {
            int p1 = 0, p2 = 0;
            for(size_t i = 0; i < top_row.size(); ++i)
            {
                p2 += top_row[i].beads.size();
                p1 += bottom_row[i].beads.size();
            }

            if(p1 == 0 && p2 != 0)
            {
                for(auto & i: top_row)
                {
                    for(auto & j: i.beads)
                        l_store.add_bead(j);
                    i.beads.clear();
                }
            }
            else if(p2 == 0 && p1 !=0)
            {
                for(auto & i: bottom_row)
                {
                    for(auto & j: i.beads)
                        r_store.add_bead(j);
                    i.beads.clear();
                }
            }
        }

        return extra_move;
    }

    // is the game over
    bool Board::finished() const
    {
        int p1 = 0, p2 = 0;
        for(size_t i = 0; i < top_row.size(); ++i)
        {
            p2 += top_row[i].beads.size();
            p1 += bottom_row[i].beads.size();
        }
        return p1 == 0 || p2 == 0;
    }

    // heuristics to evaluate the board status
    int Board::evaluate(const Mancala::Player p) const
    {
        // assume that opposite player gets all of the remaining beads at the end
        int board_count = 0;
        if(collect_rule)
            for(size_t i = 0; i < top_row.size(); ++i)
                board_count += top_row[i].beads.size() + bottom_row[i].beads.size();

        if(p == PLAYER_1)
            return r_store.beads.size() - l_store.beads.size() - board_count;
        else
            return l_store.beads.size() - r_store.beads.size() - board_count;
    }

    // ai method to choose the best move based on evaluate()
    int Board::choosemove(const Mancala::Player p) const
    {
        int best = std::numeric_limits<int>::min();
        std::vector<int> best_i;
        int score = 0;

        // loop over available moves
        if(p == PLAYER_1)
        {
            for(size_t i = 0; i < bottom_row.size(); ++i)
            {
                if(bottom_row[i].beads.size() == 0)
                    continue;
#ifdef DEBUG
                std::cout<<"p1 outer move "<<i<<std::endl;
#endif
                // create a stripped down board object to try moves on more quickly
                Simple_board sub_b = *this;
                score = 0;
                // try each move
                if(sub_b.move(PLAYER_1, i))
                {
                    score = choosemove_alphabeta(sub_b, ai_depth, PLAYER_1, std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max());
                }
                else
                {
                    score = choosemove_alphabeta(sub_b, ai_depth, PLAYER_2, std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max());
                }
#ifdef DEBUG
                std::cout<<"p1 outer move "<<i<<" score: "<<score<<std::endl;
#endif
                // keep track of the move(s) with the best score
                if(score > best)
                {
                    best = score;
                    best_i.clear();
                    best_i.push_back(i);
                }
                else if(score == best)
                    best_i.push_back(i);
            }
        }
        else
        {
            for(size_t i = 0; i < top_row.size(); ++i)
            {
                if(top_row[i].beads.size() == 0)
                    continue;
#ifdef DEBUG
                std::cout<<"p2 outer move "<<i<<std::endl;
#endif
                // create a stripped down board object to try moves on more quickly
                Simple_board sub_b = *this;
                score = 0;
                // try each move
                if(sub_b.move(PLAYER_2, i))
                {
                    score = -choosemove_alphabeta(sub_b, ai_depth, PLAYER_2, std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max());
                }
                else
                {
                    score = -choosemove_alphabeta(sub_b, ai_depth, PLAYER_1, std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max());
                }
#ifdef DEBUG
                std::cout<<"p1 outer move "<<i<<" score: "<<score<<std::endl;
#endif
                // keep track of the move(s) with the best score
                if(score > best)
                {
                    best = score;
                    best_i.clear();
                    best_i.push_back(i);
                }
                else if(score == best)
                    best_i.push_back(i);
            }
        }
#ifdef DEBUG
        std::cout<<"top scoring moves"<<std::endl;
        for(auto &i: best_i)
            std::cout<<i<<" ";
        std::cout<<std::endl;
#endif
        return best_i[rand() % best_i.size()];
    }

    std::thread::id Board::choosemove_noblock(const Mancala::Player p) const
    {
        std::thread choosemove_thread(choosemove_thread_func, std::cref(*this), p);
        std::thread::id id = choosemove_thread.get_id();
        choosemove_thread.detach();
        return id;
    }

    Board::signal_choosemove_t Board::signal_choosemove()
    {
        return signal_choosemove_sig;
    }
}
