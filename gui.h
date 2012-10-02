// gui.h
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

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
    // signal handlers

    // make a move (called by button signals)
    void move(const int i);

    // get a hint, will highlight a bowl
    void hint();

    // reset the game
    void new_game();

    // containers
    Gtk::Box main_box;
    Gtk::Box hint_box;
    Gtk::Box new_game_box;
    // stores
    // bowls
    // hint button
    Gtk::Button hint_b;
    // new game button
    Gtk::Button new_game_b;

    // Identify who's turn it is
    Gtk::Label player_label;

private:
    // update the numbers for each bowl / store
    void update_board();

    //state vars
    int player;

    // the actual mancala board
    Board b;
};

#define __MANCALA_GUI_H__
#endif // __MANCALA_GUI_H__
