// board.h
// Mancala board representation
// Copyright Matthew Chandler 2012

#ifndef MANCALA_BOARD_H
#define MANCALA_BOARD_H

#include <thread>
#include <vector>

#include <sigc++/sigc++.h>

namespace Mancala
{
    enum Player {PLAYER_1, PLAYER_2};

    // Bead data
    class Bead
    {
    public:
        Bead(const std::vector<double> & Pos = std::vector<double>({0.0, 0.0}),
            const int Color_i = 0);

        // location
        std::vector<double> pos;

        // color index
        int color_i;
    };

    // Bowl data
    class Bowl
    {
    public:
        Bowl(const int Count = 0, const std::vector<double> & Ul = std::vector<double>({0.0, 0.0}),
        const double Width = 1.0, const double Height = 1.0);

        // add a new bead
        void add_bead(const Mancala::Bead & new_bead);
        // redistribute the beads
        void redist_beads();

        // base coords for beads
        std::vector<double> ul;
        float width, height;

        // bead data
        std::vector<Bead> beads;

        Bowl * next;
        Bowl * across;
    };

    // Board data
    class Board
    {
    public:
        Board(const int Num_bowls = 6, const int Num_beads = 4, const int Ai_depth = 10,
            const bool Extra_rule = true, const bool Capture_rule = true, const bool Collect_rule = true);
        Board(const Board & b);
        Board & operator=(const Board & b);
    public:
        // set up bowls
        void set_bowls();

        // perform a move
        // returns true if the move earns an extra turn
        bool move(const Mancala::Player p, const int i);

        // is the game over
        bool finished() const;

        // heuristics to evaluate the board status
        int evaluate(const Mancala::Player p) const;

        // ai method to choose the best move based on evaluate()
        int choosemove(const Mancala::Player p) const;
        // non-blocking version
        // emits signal with int when complete
        // return id of thread, to be matched with signal
        std::thread::id choosemove_noblock(const Mancala::Player p) const;

        // signal for choosemove_noblock
        typedef sigc::signal<void, int, std::thread::id> signal_choosemove_t;
        signal_choosemove_t signal_choosemove();

        int num_bowls;
        int num_beads;
        // maximum depth for ai lookahead (choosemove)
        int ai_depth;

        // disable / enable rules
        bool extra_rule, capture_rule, collect_rule;

        // board layout vars
        std::vector<Bowl> top_row;
        std::vector<Bowl> bottom_row;
        Bowl l_store;
        Bowl r_store;

        signal_choosemove_t signal_choosemove_sig;
    };
}

#endif // MANCALA_BOARD_H
