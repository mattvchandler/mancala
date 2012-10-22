// gui.h
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#ifndef __MANCALA_GUI_H__
#define __MANCALA_GUI_H__

#include <atomic>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/window.h>

#include "mancala.h"

class Mancala_win; // predeclared to avoid circular dependency

class Mancala_bead
{
public:
    Mancala_bead(const std::vector<double> & Pos = std::vector<double>({0.0, 0.0}),
        const int Color_i = 0);
    std::vector<double> pos;
    int color_i;
};

class Mancala_bead_bowl
{
public:
    Mancala_bead_bowl(const std::vector<double> Ul = std::vector<double>({0.0, 0.0}),
        const int Num = 1, const double Width = 1.0, const double Height = 1.0);
    void add_bead(const Mancala_bead & new_bead);
    void redist_beads();

    std::vector<Mancala_bead> beads;
    std::vector<double> ul;
    float width, height;
    Mancala_bead_bowl * next;
    Mancala_bead_bowl * across;
};

enum Mancala_player {MANCALA_P1, MANCALA_P2};
class Mancala_draw: public Gtk::DrawingArea
{
public:
    Mancala_draw(Mancala_win * Win);
    void gui_move(const int i, const Mancala_player p);
    void set_gui_bowls();
    static const int num_colors = 6;
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
    Glib::RefPtr<Gdk::Pixbuf> bead_s_img;
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> bead_imgs;

    std::vector<Mancala_bead_bowl> top_row;
    std::vector<Mancala_bead_bowl> bottom_row;
    Mancala_bead_bowl l_store;
    Mancala_bead_bowl r_store;
};

// settings window
class Mancala_settings_win: public Gtk::Window
{
public:
    Mancala_settings_win(Mancala_win * Win);
protected:
    // callback
    void ok_button_func();
    void open();
    void close();

    // containers
    Gtk::Box main_box;

    // widgets
    Gtk::CheckButton p1_ai_check, p2_ai_check;
    Gtk::SpinButton board_size, board_seeds, ai_depth;
    Gtk::Label board_size_label, board_seeds_label, ai_depth_label;
    Gtk::Button ok_button, cancel_button;
private:
    Mancala_win * win;

};

class Mancala_win: public Gtk::Window
{
public:
    Mancala_win();

    friend class Mancala_draw;
    friend class Mancala_settings_win;
private:
    // the actual mancala board (must be initialized before draw)
    Board b;
protected:

    // make a move (called by button signals)
    void move(const int i);

    // AI move functions
    bool ai_timer();
    void ai_move();

    // display the winner, end the game
    void disp_winner();

    // get a hint, will highlight a bowl
    void hint();

    // reset the game
    void new_game();

    // ai menu callback
    void p1_ai_menu_f();
    void p2_ai_menu_f();

    // containers
    Gtk::Box main_box;
    // Identify who's turn it is
    Gtk::Label player_label;
    // Menu and toolbar
    Glib::RefPtr<Gtk::UIManager> uiman;
    Glib::RefPtr<Gtk::ActionGroup> actgrp;
    // Drawing area
    Mancala_draw draw;

    // AI menu items
    Glib::RefPtr<Gtk::ToggleAction> p1_ai_menu, p2_ai_menu;

    // settings window
    Mancala_settings_win settings_win;

private:
    // update the numbers for each bowl / store
    void update_board();

    // state vars
    Mancala_player player;
    bool show_hint;
    int hint_i;
    bool game_over;
    bool p1_ai, p2_ai;
    int num_bowls, num_seeds;
    int ai_depth;
    std::atomic_flag moving;
};

#endif // __MANCALA_GUI_H__
