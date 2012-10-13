// gui.h
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#ifndef __MANCALA_GUI_H__
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "mancala.h"

class Mancala_win; // predeclared to avoid circular dependency

class Mancala_draw: public Gtk::DrawingArea
{
public:
    Mancala_draw(Mancala_win * Win);
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool mouse_down(GdkEventButton * event);
private:
    Mancala_win * win;
    Glib::RefPtr<Gdk::Pixbuf> bg_store;
    Glib::RefPtr<Gdk::Pixbuf> bg_bowl;
    Glib::RefPtr<Gdk::Pixbuf> bg_board;
    Glib::RefPtr<Gdk::Pixbuf> hint_img;
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> beads;
};

class Mancala_win: public Gtk::Window
{
public:
    Mancala_win();

    friend class Mancala_draw;

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
    // hint button
    Gtk::Button hint_b;
    // new game button
    Gtk::Button new_game_b;
    // Identify who's turn it is
    Gtk::Label player_label;
    // Drawing area
    Mancala_draw draw;

private:
    // update the numbers for each bowl / store
    void update_board();

    //state vars
    int player;
    bool show_hint;
    int hint_i;

    // the actual mancala board
    Board b;
};

#define __MANCALA_GUI_H__
#endif // __MANCALA_GUI_H__
