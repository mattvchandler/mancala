// gui.cpp
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#include <iostream>
#include <sstream>

#include <gtkmm/aboutdialog.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/general.h>
#include <glibmm/fileutils.h>
#include <glibmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

#include "config.h"
#include "gui.h"

namespace Mancala
{
    Settings_win::Settings_win(Win * Win):
        p1_ai_check("Player 1 AI"),
        p2_ai_check("Player 2 AI"),
        board_size(Gtk::Adjustment::create(1.0, 1.0, 10.0)),
        board_seeds(Gtk::Adjustment::create(1.0, 1.0, 20.0)),
        ai_depth(Gtk::Adjustment::create(1.0, 0.0, 10.0)),
        board_size_label("Board size"),
        board_seeds_label("Seeds per bowl"),
        ai_depth_label("AI look-ahead"),
        extra_rule_check("Extra move"),
        capture_rule_check("Capture beads"),
        collect_rule_check("Collect beads at end"),
        full_gui_radio("Full GUI"),
        simple_gui_radio("Simple_GUI"),
        win(Win)
    {
        set_default_size(300, 150);
        set_title("Mancala Settings");
        set_transient_for(*win);
        set_modal(true);

        // layout widgets
        get_content_area()->pack_start(ai_box);
        ai_box.pack_start(ai_check_box, Gtk::PACK_EXPAND_PADDING);
        ai_check_box.pack_start(p1_ai_check);
        ai_check_box.pack_start(p2_ai_check);
        ai_box.pack_start(ai_sep);
        ai_box.pack_start(ai_depth_box, Gtk::PACK_EXPAND_PADDING);
        ai_depth_box.pack_start(ai_depth_label);
        ai_depth_box.pack_start(ai_depth);

        get_content_area()->pack_start(ai_cycles_box, Gtk::PACK_EXPAND_PADDING);
        ai_cycles_box.pack_start(ai_cycles);

        get_content_area()->pack_start(main_1_sep);

        get_content_area()->pack_start(board_box);
        board_box.pack_start(l_board_box, Gtk::PACK_EXPAND_PADDING);
        l_board_box.pack_start(board_size_label);
        l_board_box.pack_start(board_size);
        board_box.pack_start(r_board_box, Gtk::PACK_EXPAND_PADDING);
        r_board_box.pack_start(board_seeds_label);
        r_board_box.pack_start(board_seeds);

        get_content_area()->pack_start(main_2_sep);

        get_content_area()->pack_start(rule_box);
        rule_box.pack_start(extra_rule_check, Gtk::PACK_EXPAND_PADDING);
        rule_box.pack_start(capture_rule_check, Gtk::PACK_EXPAND_PADDING);
        rule_box.pack_start(collect_rule_check, Gtk::PACK_EXPAND_PADDING);

        get_content_area()->pack_start(gui_box);
        gui_box.pack_start(full_gui_radio, Gtk::PACK_EXPAND_PADDING);
        gui_box.pack_start(simple_gui_radio, Gtk::PACK_EXPAND_PADDING);

        // set tooltips
        ai_depth.set_tooltip_text("How many steps the AI looks ahead.\nMore steps will make the AI more difficult, but will make it take more time");
        ai_cycles.set_tooltip_text("Worse case number of AI look-ahead computations given board size and look-ahead");
        extra_rule_check.set_tooltip_text("Get an extra move when last bead placed in store");
        capture_rule_check.set_tooltip_text("When landing in an empty bowl, capture all beads across from it");
        collect_rule_check.set_tooltip_text("At the end of the game, players collect any beads left on their side");
        full_gui_radio.set_tooltip_text("Use a full, graphical GUI");
        simple_gui_radio.set_tooltip_text("Use a simple, button-based GUI");

        // pack the buttons
        add_button(Gtk::Stock::OK, 1);
        add_button(Gtk::Stock::CANCEL, 0);

        // set radio group
        Gtk::RadioButton::Group group = full_gui_radio.get_group();
        simple_gui_radio.set_group(group);

        show_all_children();

        // set signal handlers
        signal_response().connect(sigc::mem_fun(*this, &Settings_win::button_func));
        signal_show().connect(sigc::mem_fun(*this, &Settings_win::open));
        ai_depth.signal_value_changed().connect(sigc::mem_fun(*this, &Settings_win::ai_cycles_func));
        board_size.signal_value_changed().connect(sigc::mem_fun(*this, &Settings_win::ai_cycles_func));
    }

    // callback for settings window okay button
    void Settings_win::button_func(int response_id)
    {
        if(response_id == 1) // OK button
        {
            // update state vars with new values
            win->p1_ai = p1_ai_check.get_active();
            win->p2_ai = p2_ai_check.get_active();
            win->num_bowls = (int)board_size.get_value();
            win->num_seeds = (int)board_seeds.get_value();
            win->ai_depth = (int)ai_depth.get_value();
            win->extra_rule = extra_rule_check.get_active();
            win->capture_rule = capture_rule_check.get_active();
            win->collect_rule = collect_rule_check.get_active();

            // set gui type by setting menu item
            // this, in turn, calls its callback function which takes care of
            // switching modes
            if(full_gui_radio.get_active())
                win->full_gui_menu->set_active(true);
            else
                win->simple_gui_menu->set_active(true);

            win->new_game();
        }
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
        extra_rule_check.set_active(win->extra_rule);
        capture_rule_check.set_active(win->capture_rule);
        collect_rule_check.set_active(win->collect_rule);

        // get gui type
        if(win->full_gui)
            full_gui_radio.set_active();
        else
            simple_gui_radio.set_active();

        ai_cycles_func();
    }

    // update # of ai cycles
    void Settings_win::ai_cycles_func()
    {
        double num_cycles = pow(board_size.get_value(), ai_depth.get_value() + 1.0);
        std::ostringstream cycle_str;
        cycle_str<<"AI search space: "<<num_cycles<<" evals";
        if(num_cycles > 1e9)
            cycle_str<<" - Not Recommended";
        ai_cycles.set_text(cycle_str.str());
    }

    Win::Win():
        settings_win(this),
        player(PLAYER_1),
        game_over(false),
        p1_ai(false),
        p2_ai(true),
        num_bowls(6),
        num_seeds(4),
        ai_depth(4),
        extra_rule(true),
        capture_rule(true),
        collect_rule(true),
        full_gui(true),
        draw(num_bowls, num_seeds, ai_depth, extra_rule, capture_rule, collect_rule)
    {
        // set window properties
        set_default_size(800,400);
        set_title(MANCALA_TITLE);
        try
        {
            set_default_icon_from_file(check_in_pwd("img/icon.svg").c_str());
        }
        catch(const Glib::FileError& ex)
        {
            std::cerr<<"File Error: "<<ex.what()<<std::endl;
        }
        catch(const Gdk::PixbufError& ex)
        {
            std::cerr<<"Pixbuf Error: "<< ex.what()<<std::endl;
        }

        // build menu and toolbar
        actgrp = Gtk::ActionGroup::create();
        actgrp->add(Gtk::Action::create("Game", "Game"));
        actgrp->add(Gtk::Action::create("Game_newgame", Gtk::Stock::NEW, "_New Game", "Give up and start a new game"),
            sigc::mem_fun(*this, &Win::new_game));
        actgrp->add(Gtk::Action::create("Game_hint", Gtk::Stock::HELP, "Hint", "Get a hint."),
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

        actgrp->add(Gtk::Action::create("Display", "Display"));
        Gtk::RadioAction::Group gui_radio_group;
        full_gui_menu = Gtk::RadioAction::create(gui_radio_group, "full_gui", "Full GUI");
        actgrp->add(full_gui_menu, sigc::mem_fun(*this, &Win::gui_f));
        simple_gui_menu = Gtk::RadioAction::create(gui_radio_group, "simple_gui", "Simple GUI");
        actgrp->add(simple_gui_menu, sigc::mem_fun(*this, &Win::gui_f));

        actgrp->add(Gtk::Action::create("Help", "Help"));
        actgrp->add(Gtk::Action::create("Help_about", "About", "About"), sigc::mem_fun(*this, &Win::about));

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
        "       <menu action='Display'>"
        "           <menuitem action='full_gui'/>"
        "           <menuitem action='simple_gui'/>"
        "       </menu>"
        "       <menu action='Help'>"
        "           <menuitem action='Help_about'/>"
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

        // add widgets to containers
        add(main_box);

        Gtk::Widget * menu = uiman->get_widget("/MenuBar");
        Gtk::Widget * toolbar = uiman->get_widget("/ToolBar");

        if(menu)
        {
            main_box.pack_start(*menu, Gtk::PACK_SHRINK);
            menu->show();
        }
        if(toolbar)
        {
            main_box.pack_start(*toolbar, Gtk::PACK_SHRINK);
            toolbar->show();
        }

        main_box.pack_start(draw);
        draw.show();

        // set up simple gui elements
        main_box.pack_start(simple_gui_box);
        simple_gui_box.pack_start(simple_l_store);
        simple_gui_box.pack_end(simple_r_store);
        simple_gui_box.pack_start(simple_sub_board_box);
        simple_sub_board_box.pack_start(simple_top_row_box);
        simple_sub_board_box.pack_start(simple_bottom_row_box);

        // create and store widgets for the bowls
        // bind events to each button
        for(int i = 0; i < num_bowls; ++i)
        {
            simple_top_row_bowls.push_back(std::unique_ptr<Gtk::Button>(new Gtk::Button));
            simple_top_row_box.pack_start(*simple_top_row_bowls[i]);
            simple_top_row_bowls.back()->signal_clicked().connect(sigc::bind<Player, int>
                (sigc::mem_fun(*this, &Win::simple_button_click), PLAYER_2, i));

            simple_bottom_row_bowls.push_back(std::unique_ptr<Gtk::Button>(new Gtk::Button));
            simple_bottom_row_box.pack_start(*simple_bottom_row_bowls[i]);
            simple_bottom_row_bowls.back()->signal_clicked().connect(sigc::bind<Player, int>
                (sigc::mem_fun(*this, &Win::simple_button_click), PLAYER_1, i));
        }

        simple_gui_box.show_all_children();

        main_box.pack_end(player_label, Gtk::PACK_SHRINK);
        player_label.show();
        main_box.show();

        // set callback for mouse click in drawing area
        draw.signal_button_press_event().connect(sigc::mem_fun(*this, &Win::mouse_down));
        // set timer to make AI moves when able. Check every 500ms
        Glib::signal_timeout().connect(sigc::mem_fun(*this, &Win::ai_timer), 500);

        // set all labels for number of seeds
        update_board();
    }

    // mouse click in drawing area callback
    bool Win::mouse_down(GdkEventButton * event)
    {
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
                    ai_sig.disconnect();
                    hint_sig.disconnect();
                    if(!draw.b.move(PLAYER_1, grid_x - 1))
                        player = PLAYER_2;
                }
                else if(!p2_ai && player == PLAYER_2 && grid_y == 0 && draw.b.top_row[grid_x - 1].beads.size() > 0)
                {
                    draw.show_hint = false;
                    ai_sig.disconnect();
                    hint_sig.disconnect();
                    if(!draw.b.move(PLAYER_2, grid_x - 1))
                        player = PLAYER_1;
                }
                update_board();
            }
        }
        return true;
    }

    // check to see if AI player can move. Executed on a timer
    bool Win::ai_timer()
    {
        if(update_f.test_and_set())
            update_board();
        else
        {
            update_f.clear();
            if(!game_over && !ai_sig.connected())
            {
                if((player == PLAYER_1 && p1_ai) || (player == PLAYER_2 && p2_ai))
                {
                    draw.show_hint = false;
                    hint_sig.disconnect();
                    ai_sig = draw.b.signal_choosemove().connect(sigc::mem_fun(*this, &Win::ai_move));
                    ai_thread_id = draw.b.choosemove_noblock(player);
                }
            }
        }
        return true;
    }

    // Have the AI make a move
    void Win::ai_move(int i, std::thread::id id)
    {
        // did we get the last thread we sent off?
        if(id == ai_thread_id)
        {
            bool ai_extra_move = draw.b.move(player, i);

            if(!ai_extra_move)
            {
                if(player == PLAYER_1)
                    player = PLAYER_2;
                else
                    player = PLAYER_1;
            }

            update_f.test_and_set();
            ai_sig.disconnect();
        }
    }

    // simple gui button click
    void Win::simple_button_click(const Player p, const int i)
    {
        if(!game_over)
        {
            // determine if move is legal, and make the move
            if(!p1_ai && player == PLAYER_1 && p == PLAYER_1 && draw.b.bottom_row[i].beads.size() > 0)
            {
                draw.show_hint = false;
                ai_sig.disconnect();
                hint_sig.disconnect();
                if(!draw.b.move(PLAYER_1, i))
                    player = PLAYER_2;
            }
            else if(!p2_ai && player == PLAYER_2 && p == PLAYER_2 && draw.b.top_row[i].beads.size() > 0)
            {
                draw.show_hint = false;
                ai_sig.disconnect();
                hint_sig.disconnect();
                if(!draw.b.move(PLAYER_2, i))
                    player = PLAYER_1;
            }
            update_board();
        }
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

        Gtk::MessageDialog dlg(*this, "Game Over");
        dlg.set_secondary_text(msg);
        dlg.run();
    }

    // asynchronously get a hint, will highlight a bowl
    void Win::hint()
    {
        if(hint_sig.connected() || game_over || (player == PLAYER_1 && p1_ai) || (player == PLAYER_2 && p2_ai))
            return;
        ai_sig.disconnect();
        hint_sig = draw.b.signal_choosemove().connect(sigc::mem_fun(*this, &Win::hint_done));
        ai_thread_id = draw.b.choosemove_noblock(player);
    }

    // catch the return value of the hint
    void Win::hint_done(int i, std::thread::id id)
    {
        // did we get the last thread we sent off?
        if(id == ai_thread_id)
        {
            draw.hint_i = i;
            draw.show_hint = true;
            draw.hint_player = player;
            update_f.test_and_set();
            hint_sig.disconnect();
        }
    }

    // start a new game
    void Win::new_game()
    {
        // Reactivate hint feature
        game_over = false;
        actgrp->get_action("Game_hint")->set_sensitive(true);

        player = PLAYER_1;
        draw.b = Board(num_bowls, num_seeds, ai_depth, extra_rule, capture_rule, collect_rule);

        // rebuild simple_gui
        for(size_t i = 0; i < simple_top_row_bowls.size(); ++i)
        {
            simple_top_row_box.remove(*simple_top_row_bowls[i]);
            simple_bottom_row_box.remove(*simple_bottom_row_bowls[i]);
        }

        simple_top_row_bowls.clear();
        simple_bottom_row_bowls.clear();

        // create and store widgets for the bowls
        // bind events to each button
        for(int i = 0; i < num_bowls; ++i)
        {
            simple_top_row_bowls.push_back(std::unique_ptr<Gtk::Button>(new Gtk::Button));
            simple_top_row_box.pack_start(*simple_top_row_bowls[i]);
            simple_top_row_bowls.back()->signal_clicked().connect(sigc::bind<Player, int>
                (sigc::mem_fun(*this, &Win::simple_button_click), PLAYER_2, i));

            simple_bottom_row_bowls.push_back(std::unique_ptr<Gtk::Button>(new Gtk::Button));
            simple_bottom_row_box.pack_start(*simple_bottom_row_bowls[i]);
            simple_bottom_row_bowls.back()->signal_clicked().connect(sigc::bind<Player, int>
                (sigc::mem_fun(*this, &Win::simple_button_click), PLAYER_1, i));
        }

        simple_gui_box.show_all_children();

        draw.show_hint = false;
        ai_sig.disconnect();
        hint_sig.disconnect();
        update_board();
    }

    // update the numbers for each bowl / store
    void Win::update_board()
    {
        // Show whose turn it is
        if(player == PLAYER_1)
            player_label.set_text("Player 1");
        else
            player_label.set_text("Player 2");

        // update ai menu items
        p1_ai_menu->set_active(p1_ai);
        p2_ai_menu->set_active(p2_ai);

        // call for a redraw
        if(full_gui)
            draw.queue_draw();
        else
        {
            // update labels and buttons for simple gui
            std::ostringstream l_store_str, r_store_str;
            l_store_str<<draw.b.l_store.beads.size();
            simple_l_store.set_text(l_store_str.str());

            r_store_str<<draw.b.r_store.beads.size();
            simple_r_store.set_text(r_store_str.str());

            for(int i = 0; i < num_bowls; ++i)
            {
                std::ostringstream top_count_str, bottom_count_str;
                top_count_str<<draw.b.top_row[i].beads.size();
                simple_top_row_bowls[i]->set_label(top_count_str.str());

                bottom_count_str<<draw.b.bottom_row[i].beads.size();
                simple_bottom_row_bowls[i]->set_label(bottom_count_str.str());

                // clear hint highlighting
                simple_top_row_bowls[i]->drag_unhighlight();
                simple_bottom_row_bowls[i]->drag_unhighlight();
            }

            // set hint highlighting
            if(draw.show_hint)
            {
                if(draw.hint_player == PLAYER_1)
                    simple_bottom_row_bowls[draw.hint_i]->drag_highlight();
                else
                    simple_top_row_bowls[draw.hint_i]->drag_highlight();
            }
        }

        // check to see if the game is over
        if(!game_over && draw.b.finished())
            disp_winner();

        update_f.clear();
    }

    // AI menu callbacks
    void Win::p1_ai_menu_f()
    {
        p1_ai = p1_ai_menu->get_active();
        ai_sig.disconnect();
        hint_sig.disconnect();
    }

    void Win::p2_ai_menu_f()
    {
        p2_ai = p2_ai_menu->get_active();
        ai_sig.disconnect();
        hint_sig.disconnect();
    }

    // GUI menu callback
    void Win::gui_f()
    {
        if(full_gui_menu->get_active() && !full_gui)
        {
            full_gui = true;
            draw.show();
            simple_gui_box.hide();
            resize(800,400);
            update_board();
        }
        else if(simple_gui_menu->get_active() && full_gui)
        {
            full_gui = false;
            draw.hide();
            simple_gui_box.show();
            resize(200,150);
            update_board();
        }
    }

    // about menu callback
    void Win::about()
    {
        Gtk::AboutDialog about;
        about.set_modal(true);
        about.set_transient_for(*this);

        about.set_program_name(MANCALA_TITLE);
        about.set_version(VERSION_MAJOR_STR "." VERSION_MINOR_STR);
        about.set_copyright(u8"© 2014 Matthew Chandler");
        about.set_comments(MANCALA_SUMMARY);
        about.set_license_type(Gtk::LICENSE_MIT_X11);
        about.set_website(MANCALA_WEBSITE);
        about.set_website_label(MANCALA_TITLE " on Github");
        about.set_authors({MANCALA_AUTHOR});
        about.set_artists({MANCALA_AUTHOR});
        about.set_documenters({MANCALA_AUTHOR});
        about.run();
    }
}
