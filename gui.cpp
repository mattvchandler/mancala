// gui.cpp
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#include <sstream>

#include "gui.h"


Mancala_win::Mancala_win():
    main_box(Gtk::ORIENTATION_VERTICAL),
    board_box(Gtk::ORIENTATION_HORIZONTAL),
    sub_board_box(Gtk::ORIENTATION_VERTICAL),
    top_row_box(Gtk::ORIENTATION_HORIZONTAL),
    bottom_row_box(Gtk::ORIENTATION_HORIZONTAL),
    l_store("0"),
    r_store("0"),
    player_label("Player 1")
{

    // set window properties
    set_border_width(10);
    set_default_size(200,100);
    set_title("Mancala");

    // add widgets to contatiners
    add(main_box);
    main_box.pack_start(player_label, Gtk::PACK_SHRINK);
    main_box.pack_end(board_box);

    board_box.pack_start(l_store);
    board_box.pack_end(r_store);

    board_box.pack_start(sub_board_box);
    sub_board_box.pack_start(top_row_box);
    sub_board_box.pack_end(bottom_row_box);

    // create and store widgets for the bowls
    // bind events to each button
    for(int i = 0; i < b.num_bowls; ++i)
    {
        top_row_bowls.push_back(std::unique_ptr<Gtk::Label> (new Gtk::Label));
        top_row_box.pack_start(*top_row_bowls[i]);

        bottom_row_bowls.push_back(std::unique_ptr<Gtk::Button> (new Gtk::Button));
        bottom_row_box.pack_start(*bottom_row_bowls[i]);
        bottom_row_bowls.back()->signal_clicked().connect(sigc::mem_fun(*this, &Mancala_win::move));// TODO: pass i as param
    }
    update_board();
    show_all_children();
}

// make a move (called by button signals)
void Mancala_win::move()
{
}

// update the numbers for each bowl / store
void Mancala_win::update_board()
{
    // get and update the counts for the stores
    std::ostringstream l_store_count_str;
    l_store_count_str<<b.bowls[b.p1_store].count;
    l_store.set_text(l_store_count_str.str());

    std::ostringstream r_store_count_str;
    r_store_count_str<<b.bowls[b.p2_store].count;
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
    }
}
