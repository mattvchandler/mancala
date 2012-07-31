// gui.cpp
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#include <sstream>

#include <gtkmm/messagedialog.h>

#include "gui.h"


Mancala_win::Mancala_win():
    main_box(Gtk::ORIENTATION_VERTICAL),
    board_box(Gtk::ORIENTATION_HORIZONTAL),
    sub_board_box(Gtk::ORIENTATION_VERTICAL),
    top_row_box(Gtk::ORIENTATION_HORIZONTAL),
    bottom_row_box(Gtk::ORIENTATION_HORIZONTAL),
    hint_box(Gtk::ORIENTATION_HORIZONTAL),
    hint_b("Hint"),
    player(1),
    playable(true)
{

    // set window properties
    set_border_width(10);
    set_default_size(200,100);
    set_title("Mancala");

    // add widgets to contatiners
    add(main_box);
    main_box.pack_start(player_label, Gtk::PACK_SHRINK);
    main_box.pack_start(board_box);

    board_box.pack_start(l_store);
    board_box.pack_end(r_store);

    board_box.pack_start(sub_board_box);
    sub_board_box.pack_start(top_row_box);
    sub_board_box.pack_end(bottom_row_box);

    main_box.pack_end(hint_box, Gtk::PACK_SHRINK);
    hint_box.pack_start(hint_b, Gtk::PACK_EXPAND_PADDING);

    // create and store widgets for the bowls
    // bind events to each button
    for(int i = 0; i < b.num_bowls; ++i)
    {
        top_row_bowls.push_back(std::unique_ptr<Gtk::Label> (new Gtk::Label));
        top_row_box.pack_start(*top_row_bowls[i]);

        bottom_row_bowls.push_back(std::unique_ptr<Gtk::Button> (new Gtk::Button));
        bottom_row_box.pack_start(*bottom_row_bowls[i]);
        bottom_row_bowls.back()->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &Mancala_win::move), i));
    }

    hint_b.signal_clicked().connect(sigc::mem_fun(*this, &Mancala_win::hint));

    // set all labels for number of seeds
    update_board();

    show_all_children();
}

// make a move (called by button signals)
void Mancala_win::move(const int i)
{
    if(!playable)
        return;

    if(b.bowls[b.p1_start + i].count <= 0)
        return;

    if(!b.move(i))
    {
        player = (player == 1)? 2 : 1;
        b.swapsides();
    }

    update_board();

    // check to see if the game is over
    if(b.finished())
    {
        Glib::ustring msg;
        // check for a tie
        if(b.bowls[b.p1_store].count == b.bowls[b.p2_store].count)
            msg = "Tie";

        // determine the current player, and then see if they won or lost
        else if(player == 1)
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                msg = "Player 1 wins";
            else
                msg = "Player 2 wins";
        else
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                msg = "Player 2 wins";
            else
                msg = "Player 1 wins";

        // was the win full of win? 
        if(abs(b.bowls[b.p1_store].count - b.bowls[b.p2_store].count) >= 10)
            msg += "\nFATALITY";

        //create and show a dialog announcing the winner
        Gtk::MessageDialog dlg(*this, "Game Over");
        dlg.set_secondary_text(msg);
        dlg.run();

        //mark the game as unplayable
        playable = false;
    }
}


// get a hint, will highlight a bowl
void Mancala_win::hint()
{
    int bestmove = choosemove(b);
    bottom_row_bowls[bestmove]->drag_highlight();
}

// update the numbers for each bowl / store
void Mancala_win::update_board()
{
    // Show who's turn it is TODO: ugly
    if(player == 1)
        player_label.set_text("Player 1");
    else
        player_label.set_text("Player 2");

    // get and update the counts for the stores
    std::ostringstream l_store_count_str;
    l_store_count_str<<b.bowls[b.p2_store].count;
    l_store.set_text(l_store_count_str.str());

    std::ostringstream r_store_count_str;
    r_store_count_str<<b.bowls[b.p1_store].count;
    r_store.set_text(r_store_count_str.str());

    for(int i = 0; i < b.num_bowls; ++i)
    {
        std::ostringstream top_count_str;
        top_count_str.clear();
        top_count_str<<b.bowls[b.bowls[b.p1_start + i].across].count;
        top_row_bowls[i]->set_text(top_count_str.str());

        std::ostringstream bottom_count_str;
        bottom_count_str.clear();
        bottom_count_str<<b.bowls[b.p1_start + i].count;
        bottom_row_bowls[i]->set_label(bottom_count_str.str());

        // clear any hint highlighting
        bottom_row_bowls[i]->drag_unhighlight();
    }
}
