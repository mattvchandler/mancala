// gui.cpp
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#include <iostream>

#include <gdkmm/general.h>
#include <glibmm/fileutils.h>
#include <glibmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

#include "gui.h"

namespace Mancala
{
    Settings_win::Settings_win(Win * Win):
        main_box(Gtk::ORIENTATION_VERTICAL),
        p1_ai_check("Player 1 AI"),
        p2_ai_check("Player 2 AI"),
        board_size(Gtk::Adjustment::create(1.0, 1.0, 10.0)),
        board_seeds(Gtk::Adjustment::create(1.0, 1.0, 20.0)),
        ai_depth(Gtk::Adjustment::create(1.0, 1.0, 10.0)),
        board_size_label("Board size"),
        board_seeds_label("Seeds per bowl"),
        ai_depth_label("AI look-ahead steps"),
        ok_button(Gtk::Stock::OK),
        cancel_button(Gtk::Stock::CANCEL),
        win(Win)
    {
        set_title("Mancala Board Settings");

        // layot widgets
        add(main_box);

        main_box.pack_start(p1_ai_check);
        main_box.pack_start(p2_ai_check);
        main_box.pack_start(board_size);
        main_box.pack_start(board_size_label);
        main_box.pack_start(board_seeds);
        main_box.pack_start(board_seeds_label);
        main_box.pack_start(ai_depth);
        main_box.pack_start(ai_depth_label);
        main_box.pack_start(ok_button);
        main_box.pack_start(cancel_button);

        show_all_children();

        // set signal handlers
        signal_show().connect(sigc::mem_fun(*this, &Settings_win::open));
        signal_hide().connect(sigc::mem_fun(*this, &Settings_win::close));
        ok_button.signal_clicked().connect(sigc::mem_fun(*this, &Settings_win::ok_button_func));
        cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &Settings_win::hide));
    }

    // callback for settings window okay button
    void Settings_win::ok_button_func()
    {
        // update state vars with new values
        win->p1_ai = p1_ai_check.get_active();
        win->p2_ai = p2_ai_check.get_active();
        win->num_bowls = (int)board_size.get_value();
        win->num_seeds = (int)board_seeds.get_value();
        win->ai_depth = (int)ai_depth.get_value();
        win->new_game();
        hide();
    }

    // callback for settings window open
    void Settings_win::open()
    {
        // set spinner values
        board_size.set_value((double)win->num_bowls);
        board_seeds.set_value((double)win->num_seeds);
        ai_depth.set_value((double)win->ai_depth);
        // set checkbox values
        p1_ai_check.set_active(win->p1_ai);
        p2_ai_check.set_active(win->p2_ai);

        // disavle main window
        win->set_sensitive(false);
    }

    // callback for settings window close
    void Settings_win::close()
    {
        win->set_sensitive(true);
    }

    Win::Win():
        main_box(Gtk::ORIENTATION_VERTICAL),
        settings_win(this),
        player(PLAYER_1),
        game_over(false),
        p1_ai(false),
        p2_ai(true),
        num_bowls(6),
        num_seeds(4),
        ai_depth(10),
        draw(num_bowls, num_seeds, ai_depth)
    {
        moving.test_and_set();

        // set window properties
        set_default_size(800,400);
        set_title("Mancala");

        // build menu and toolbar
        actgrp = Gtk::ActionGroup::create();
        actgrp->add(Gtk::Action::create("Game", "Game"));
        actgrp->add(Gtk::Action::create("Game_newgame", Gtk::Stock::NEW, "_New Game", "Give up and start a new game"),
            sigc::mem_fun(*this, &Win::new_game));
        actgrp->add(Gtk::Action::create("Game_hint", Gtk::Stock::HELP, "Hint", "Get a hint. May be misleading"),
            sigc::mem_fun(*this, &Win::hint));
        actgrp->add(Gtk::Action::create("Game_set", Gtk::Stock::PREFERENCES, "_Settings", "Game settings"),
            sigc::mem_fun(settings_win, &Settings_win::show));
        actgrp->add(Gtk::Action::create("Game_quit", Gtk::Stock::QUIT, "_Quit", "Quit"),
            sigc::mem_fun(*this, &Win::hide));
        actgrp->add(Gtk::Action::create("Players", "Players"));
        p1_ai_menu = Gtk::ToggleAction::create("Players_1_ai", "Player 1 AI", "Toggle Player 1 AI", p1_ai);
        p2_ai_menu = Gtk::ToggleAction::create("Players_2_ai", "Player 2 AI", "Toggle Player 2 AI", p2_ai);
        actgrp->add(p1_ai_menu, sigc::mem_fun(*this, &Win::p1_ai_menu_f));
        actgrp->add(p2_ai_menu, sigc::mem_fun(*this, &Win::p2_ai_menu_f));

        uiman = Gtk::UIManager::create();
        uiman->insert_action_group(actgrp);
        add_accel_group(uiman->get_accel_group());

        Glib::ustring ui_str =
        "<ui>"
        "   <menubar name='MenuBar'>"
        "       <menu action='Game'>"
        "           <menuitem action='Game_newgame'/>"
        "           <menuitem action='Game_hint'/>"
        "           <menuitem action='Game_set'/>"
        "           <separator/>"
        "           <menuitem action='Game_quit'/>"
        "       </menu>"
        "       <menu action='Players'>"
        "           <menuitem action='Players_1_ai'/>"
        "           <menuitem action='Players_2_ai'/>"
        "       </menu>"
        "   </menubar>"
        "   <toolbar name='ToolBar'>"
        "       <toolitem action='Game_newgame'/>"
        "       <toolitem action='Game_hint'/>"
        "       <toolitem action='Game_set'/>"
        "   </toolbar>"
        "</ui>";

        try
        {
            uiman->add_ui_from_string(ui_str);
        }
        catch(const Glib::Error& ex)
        {
            std::cerr<<"Menu Error: "<<ex.what()<<std::endl;
        }

        // add widgets to contatiners
        add(main_box);

        Gtk::Widget * menu = uiman->get_widget("/MenuBar");
        Gtk::Widget * toolbar = uiman->get_widget("/ToolBar");

        if(menu)
            main_box.pack_start(*menu, Gtk::PACK_SHRINK);
        if(toolbar)
            main_box.pack_start(*toolbar, Gtk::PACK_SHRINK);

        main_box.pack_start(draw);
        main_box.pack_end(player_label, Gtk::PACK_SHRINK);

        // set callback for mouse click in drawing area
        draw.signal_button_press_event().connect(sigc::mem_fun(*this, &Win::mouse_down));
        // set timer to make AI moves when able. Check every 50ms
        Glib::signal_timeout().connect(sigc::mem_fun(*this, &Win::ai_timer), 50);

        // set all labels for number of seeds
        update_board();

        show_all_children();
        moving.clear();
    }

    // mouse click in drawing area callback
    bool Win::mouse_down(GdkEventButton * event)
    {
        if(moving.test_and_set())
            return true;
        if(!game_over)
        {
            Gtk::Allocation alloc = draw.get_allocation();

            // determine which bowl was clicked
            int grid_x = (int)((num_bowls + 2) * event->x / alloc.get_width());
            int grid_y = (event->y / alloc.get_height() <= .5)? 0 : 1;

            // determine if move is legal, and make the move
            if(grid_x > 0 && grid_x < num_bowls + 1)
            {
                if(!p1_ai && player == PLAYER_1 && grid_y == 1 && draw.b.bottom_row[grid_x - 1].beads.size() > 0)
                {
                    draw.show_hint = false;
                    if(!draw.b.move(PLAYER_1, grid_x - 1))
                        player = PLAYER_2;
                }
                else if(!p2_ai && player == PLAYER_2 && grid_y == 0 && draw.b.top_row[grid_x - 1].beads.size() > 0)
                {
                    draw.show_hint = false;
                    if(!draw.b.move(PLAYER_2, grid_x - 1))
                        player = PLAYER_1;
                }

                // is the game over?
                if(draw.b.finished())
                    disp_winner();

                update_board();
            }
        }
        moving.clear();
        return true;
    }

    // check to see if AI player can move. executed on a timer
    bool Win::ai_timer()
    {
        if(moving.test_and_set())
            return true;
        if(!game_over)
        {
            if((player == PLAYER_1 && p1_ai) || (player == PLAYER_2 && p2_ai))
                ai_move();
        }
        moving.clear();
        return true;
    }

    // Have the AI make a move
    void Win::ai_move()
    {
        bool ai_extra_move = false;

        int ai_move = draw.b.choosemove(player);
        ai_extra_move = draw.b.move(player, ai_move);

        if(!ai_extra_move)
        {
            if(player == PLAYER_1)
                player = PLAYER_2;
            else
                player = PLAYER_1;
        }

        // check to see if the game is over
        if(draw.b.finished())
            disp_winner();

        draw.show_hint = false;

        update_board();
    }

    // display the winner, end the game
    void Win::disp_winner()
    {
        // deactivate hint feature
        game_over = true;
        actgrp->get_action("Game_hint")->set_sensitive(false);

        // create and show a dialog announcing the winner
        Glib::ustring msg;

        // check for a tie
        if(draw.b.l_store.beads.size() == draw.b.r_store.beads.size())
            msg = "Tie";
        else
            if(draw.b.r_store.beads.size() > draw.b.l_store.beads.size())
                msg = "Player 1 wins";
            else
                msg = "Player 2 wins";

        // was the win full of win?
        if(abs(draw.b.r_store.beads.size() - draw.b.l_store.beads.size()) >= 10)
            msg += "\nFATALITY";

        Gtk::MessageDialog dlg(*this, "Game Over");
        dlg.set_secondary_text(msg);
        dlg.run();
    }

    // get a hint, will highlight a bowl
    void Win::hint()
    {
        if(game_over)
            return;

        draw.hint_i = draw.b.choosemove(player);
        draw.show_hint = true;
        draw.hint_player =player;
        update_board();
    }

    // start a new game
    void Win::new_game()
    {
        // Reactivate hint feature
        game_over = false;
        actgrp->get_action("Game_hint")->set_sensitive(true);

        player = PLAYER_1;
        draw.b = Board(num_bowls, num_seeds, ai_depth);
        draw.show_hint = false;
        update_board();
    }

    // update the numbers for each bowl / store
    void Win::update_board()
    {
        // Show who's turn it is TODO: ugly
        if(player == PLAYER_1)
            player_label.set_text("Player 1");
        else
            player_label.set_text("Player 2");

        // update ai menu items
        p1_ai_menu->set_active(p1_ai);
        p2_ai_menu->set_active(p2_ai);

        // call for a redraw
        draw.queue_draw();
    }

    // AI menu callbacks
    void Win::p1_ai_menu_f()
    {
        p1_ai = p1_ai_menu->get_active();
    }

    void Win::p2_ai_menu_f()
    {
        p2_ai = p2_ai_menu->get_active();
    }
}
