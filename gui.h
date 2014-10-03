// gui.h
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2014

#ifndef MANCALA_GUI_H
#define MANCALA_GUI_H

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <gtkmm/window.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/separator.h>
#include <gtkmm/radiomenuitem.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/label.h>
#include <gtkmm/dialog.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>

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
        Gtk::HBox gui_box;

        Gtk::HSeparator main_1_sep, main_2_sep;
        Gtk::VSeparator ai_sep;

        // widgets
        Gtk::CheckButton p1_ai_check, p2_ai_check;
        Gtk::SpinButton board_size, board_seeds, ai_depth;
        Gtk::Label board_size_label, board_seeds_label, ai_depth_label;
        Gtk::Label ai_cycles;
        Gtk::CheckButton extra_rule_check, capture_rule_check, collect_rule_check;
        Gtk::RadioButton full_gui_radio, simple_gui_radio;

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
        // simple gui button click
        void simple_button_click(const Player p, const int i);

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
        void gui_f();

        // about menu callback
        void about();

        // containers
        Gtk::VBox main_box;

        // simple gui elements
        Gtk::HBox simple_gui_box;
        Gtk::VBox simple_sub_board_box;
        Gtk::HBox simple_top_row_box;
        Gtk::HBox simple_bottom_row_box;
        Gtk::Label simple_l_store, simple_r_store;
        std::vector<std::unique_ptr<Gtk::Button>> simple_top_row_bowls;
        std::vector<std::unique_ptr<Gtk::Button>> simple_bottom_row_bowls;

        // Identify who's turn it is
        Gtk::Label player_label;
        // AI menu items
        std::unique_ptr<Gtk::CheckMenuItem> players_1_ai, players_2_ai;

        // GUI selection menu item
        std::unique_ptr<Gtk::RadioMenuItem> display_full_gui, display_simple_gui;

        // hint items (saved so we can disable them)
        std::unique_ptr<Gtk::MenuItem> game_hint;
        std::unique_ptr<Gtk::ToolButton> hint_button;

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
#endif // MANCALA_GUI_H
