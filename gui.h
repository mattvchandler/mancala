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

class Mancala_bead
{
public:
    Mancala_bead(const std::vector<double> & Pos = std::vector<double>({0.0, 0.0}),
        const std::vector<double> & Color = std::vector<double>({0.0, 0.0, 0.0}));
    std::vector<double> pos;
    std::vector<double> color;
};

class Mancala_bead_bowl
{
public:
    Mancala_bead_bowl(const std::vector<double> Center = std::vector<double>({0.0, 0.0}),
        const int Num = 1, const double width = 1.0, const double height = 1.0);

    std::vector<Mancala_bead> beads;
    std::vector<double> ul;
    int num;
};

class Mancala_draw: public Gtk::DrawingArea
{
public:
    Mancala_draw(Mancala_win * Win);
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool mouse_down(GdkEventButton * event);
private:
    void draw_img(const Cairo::RefPtr<Cairo::Context>& cr, const Glib::RefPtr<Gdk::Pixbuf>&  img,
        const double translate_x, const double translate_y, const double scale_x, const double scale_y);
    void draw_num(const Cairo::RefPtr<Cairo::Context>& cr, const Pango::FontDescription & font, const int num,
        const double x, const double y);
    Mancala_win * win;
    Glib::RefPtr<Gdk::Pixbuf> bg_store;
    Glib::RefPtr<Gdk::Pixbuf> bg_bowl;
    Glib::RefPtr<Gdk::Pixbuf> bg_board;
    Glib::RefPtr<Gdk::Pixbuf> hint_img;
    Glib::RefPtr<Gdk::Pixbuf> bead_img;
    Glib::RefPtr<Gdk::Pixbuf> bead_s_img;

    std::vector<Mancala_bead_bowl> top_row;
    std::vector<Mancala_bead_bowl> bottom_row;
    Mancala_bead_bowl l_store;
    Mancala_bead_bowl r_store;
};

class Mancala_win: public Gtk::Window
{
public:
    Mancala_win();

    friend class Mancala_draw;

private:
    // the actual mancala board (must be initialized before draw)
    Board b;
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
};

#define __MANCALA_GUI_H__
#endif // __MANCALA_GUI_H__
