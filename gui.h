// gui.h
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#ifndef __MANCALA_GUI_H__
#define __MANCALA_GUI_H__

#include <atomic>
#include <thread>
#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/separator.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/window.h>

#include "board.h"
#include "draw.h"

namespace Mancala
{
    class Win; // predeclared to avoid circular dependency

    // settings window
    class Settings_win: public Gtk::Dialog
    {
    public:
        Settings_win(Win * Win);

        // callback for settings window OK button
        void button_func(int response_id);
        // callback for settings window open
        void open();
        // update # of ai cycles
        void ai_cycles_func();

        // containers
        Gtk::HBox ai_box;
        Gtk::VBox ai_check_box;
        Gtk::VBox ai_depth_box;
        Gtk::HBox ai_cycles_box;
        Gtk::HBox board_box;
        Gtk::VBox l_board_box;
        Gtk::VBox r_board_box;
        Gtk::HBox rule_box;

        Gtk::HSeparator main_1_sep, main_2_sep;
        Gtk::VSeparator ai_sep;

        // widgets
        Gtk::CheckButton p1_ai_check, p2_ai_check;
        Gtk::CheckButton extra_rule_check, capture_rule_check, collect_rule_check;
        Gtk::SpinButton board_size, board_seeds, ai_depth;
        Gtk::Label board_size_label, board_seeds_label, ai_depth_label;
        Gtk::Label ai_cycles;
    private:
        Win * win;
    };

    class Win: public Gtk::Window
    {
    public:
        Win();

        friend class Settings_win;

        // mouse click in drawing area callback
        bool mouse_down(GdkEventButton * event);
        // AI move functions
        bool ai_timer();
        void ai_move(int i, std::thread::id id);

        // display the winner, end the game
        void disp_winner();

        // get a hint, will highlight a bowl
        void hint();
        void hint_done(int i, std::thread::id id);

        // reset the game
        void new_game();

        // ai menu callbacks
        void p1_ai_menu_f();
        void p2_ai_menu_f();

        // GUI menu callbacks
        void full_gui_f();
        void simple_gui_f();

        // containers
        Gtk::Box main_box;
        // Identify who's turn it is
        Gtk::Label player_label;
        // Menu and toolbar
        Glib::RefPtr<Gtk::UIManager> uiman;
        Glib::RefPtr<Gtk::ActionGroup> actgrp;

        // AI menu items
        Glib::RefPtr<Gtk::ToggleAction> p1_ai_menu, p2_ai_menu;

        // GUI selection menu item
        Glib::RefPtr<Gtk::RadioAction> full_gui_menu, simple_gui_menu;

        // settings window
        Settings_win settings_win;

    private:
        // update the numbers for each bowl / store
        void update_board();

        // state vars
        Player player;
        bool game_over;
        sigc::connection hint_sig;
        sigc::connection ai_sig;

        bool p1_ai, p2_ai;
        int num_bowls, num_seeds;
        int ai_depth;
        bool extra_rule, capture_rule, collect_rule;
        bool full_gui;

        // flag set when update_board needs called
        std::atomic_flag update_f;

        // id of the thread running the ai search.
        // makes sure we get the result one back
        std::thread::id ai_thread_id;

        // Drawing area
        Draw draw;
    };
}
#endif // __MANCALA_GUI_H__
