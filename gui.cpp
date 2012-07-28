//gui.cpp
//GUI for mancala game, using gtkmm
//Copyright Matthew Chandler 2012

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

    //add widgets to contatiners
    add(main_box);
    main_box.pack_start(player_label, Gtk::PACK_SHRINK);
    main_box.pack_end(board_box);

    board_box.pack_start(l_store);
    board_box.pack_end(r_store);

    board_box.pack_start(sub_board_box);
    sub_board_box.pack_start(top_row_box);
    sub_board_box.pack_end(bottom_row_box);

    //create and store widgets for the bowls
    std::ostringstream seed_str;
    seed_str<<b.num_seeds;

    for(int i = 0; i < b.num_bowls; i++)
    {
        top_row_bowls.push_back(std::unique_ptr<Gtk::Label> (new Gtk::Label(seed_str.str())));
        top_row_box.pack_start(*top_row_bowls[i]);

        bottom_row_bowls.push_back(std::unique_ptr<Gtk::Button> (new Gtk::Button(seed_str.str())));
        bottom_row_box.pack_start(*bottom_row_bowls[i]);
    }

    show_all_children();
}
