//gui.h
//GUI for mancala game, using gtkmm
//Copyright Matthew Chandler 2012

#ifndef __MANCALA_GUI_H__
#include <memory>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "mancala.h"

class Mancala_win: public Gtk::Window
{
public:
    Mancala_win();

protected:
    //containers
    Gtk::Box main_box;
    Gtk::Box board_box;
    Gtk::Box sub_board_box;
    Gtk::Box top_row_box;
    Gtk::Box bottom_row_box;
    //stores
    Gtk::Label l_store, r_store;
    //bowls
    std::vector<std::unique_ptr<Gtk::Label>> top_row_bowls;
    std::vector<std::unique_ptr<Gtk::Button>> bottom_row_bowls;

    //Identify who's turn it is
    Gtk::Label player_label;

private:
    //the actual mancala board
    Board b;
};

#define __MANCALA_GUI_H__
#endif //__MANCALA_GUI_H__
