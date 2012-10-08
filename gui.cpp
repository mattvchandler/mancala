// gui.cpp
// GUI for mancala game, using gtkmm
// Copyright Matthew Chandler 2012

#include <iostream>

#include <gdkmm/general.h>
#include <glibmm/fileutils.h>
#include <gtkmm/messagedialog.h>

#include "gui.h"

Mancala_draw::Mancala_draw(Mancala_win * Win): win(Win)
{
    add_events(Gdk::BUTTON_PRESS_MASK);
    signal_button_press_event().connect(sigc::mem_fun(*this, &Mancala_draw::mouse_down));
    try
    {
        bg_store = Gdk::Pixbuf::create_from_file("img/bg_store.png");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr<<"FileError: "<<ex.what()<<std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
        std::cerr<<"PixbufError: "<< ex.what()<<std::endl;
    }
}

bool Mancala_draw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gtk::Allocation alloc = get_allocation();

    cr->save();
    cr->scale(alloc.get_width() / (double)bg_store->get_width() / (win->b.num_bowls + 2), alloc.get_height() / (double)bg_store->get_height());
    Gdk::Cairo::set_source_pixbuf(cr, bg_store);
    cr->paint();
    cr->restore();

    cr->save();
    cr->translate(alloc.get_width() * (1.0 - 1.0 / (win->b.num_bowls + 2)), 0);
    cr->scale(alloc.get_width() / (double)bg_store->get_width() / (win->b.num_bowls + 2), alloc.get_height() / (double)bg_store->get_height());
    Gdk::Cairo::set_source_pixbuf(cr, bg_store);
    cr->paint();
    cr->restore();

    cr->set_source_rgb(1.0, 0.0, 0.0);

    for(int i = 1; i < win->b.num_bowls + 2; ++i)
    {
        cr->move_to(alloc.get_width() * (i / (double)(win->b.num_bowls + 2)), 0.0);
        cr->line_to(alloc.get_width() * (i / (double)(win->b.num_bowls + 2)), alloc.get_height());
        cr->stroke();
    }

    cr->move_to(1.0 / (win->b.num_bowls + 2) * alloc.get_width(), alloc.get_height() * .5);
    cr->line_to((1.0 -  1.0 / (win->b.num_bowls + 2)) * alloc.get_width(), alloc.get_height() * .5);
    cr->stroke();

    Pango::FontDescription font("Monospace");
    font.set_size(std::min(alloc.get_width(), alloc.get_height()) * .1 * Pango::SCALE);
    int tex_w, tex_h;
    cr->set_source_rgb(0.0, 0.0, 0.0);

    // draw # for left store
    std::ostringstream l_store_str;
    l_store_str<<win->b.bowls[win->b.p2_store].count;
    Glib::RefPtr<Pango::Layout> l_store_txt = create_pango_layout(l_store_str.str());
    l_store_txt->set_font_description(font);
    l_store_txt->get_pixel_size(tex_w, tex_h);
    cr->move_to(alloc.get_width() * 1.0 / (2.0 * (win->b.num_bowls + 2)) - tex_w * .5, (alloc.get_height() - tex_h) * .5);
    l_store_txt->show_in_cairo_context(cr);

    // draw # for right store
    std::ostringstream r_store_str;
    r_store_str<<win->b.bowls[win->b.p1_store].count;
    Glib::RefPtr<Pango::Layout> r_store_txt = create_pango_layout(r_store_str.str());
    r_store_txt->set_font_description(font);
    r_store_txt->get_pixel_size(tex_w, tex_h);
    cr->move_to(alloc.get_width() * (1.0 - 1.0 / (2.0 * (win->b.num_bowls + 2))) - tex_w * .5, (alloc.get_height() - tex_h) * .5);
    r_store_txt->show_in_cairo_context(cr);

    // draw #s for bowls
    for(int i = 0; i < win->b.num_bowls; ++i)
    {
        //upper row
        std::ostringstream upper_str;
        upper_str<<win->b.bowls[win->b.bowls[win->b.p1_start + i].across].count;
        Glib::RefPtr<Pango::Layout> upper_txt = create_pango_layout(upper_str.str());
        upper_txt->set_font_description(font);
        upper_txt->get_pixel_size(tex_w, tex_h);
        cr->move_to(alloc.get_width() * (2 * i + 3) / (2.0 * (win->b.num_bowls + 2)) - tex_w * .5, alloc.get_height() * .25 - tex_h * .5);
        upper_txt->show_in_cairo_context(cr);

        //lower row
        std::ostringstream lower_str;
        lower_str<<win->b.bowls[win->b.p1_start + i].count;
        Glib::RefPtr<Pango::Layout> lower_txt = create_pango_layout(lower_str.str());
        lower_txt->set_font_description(font);
        lower_txt->get_pixel_size(tex_w, tex_h);
        cr->move_to(alloc.get_width() * (2 * i + 3) / (2.0 * (win->b.num_bowls + 2)) - tex_w * .5, alloc.get_height() * .75 - tex_h * .5);
        lower_txt->show_in_cairo_context(cr);
    }

    return true;
}

bool Mancala_draw::mouse_down(GdkEventButton * event)
{
    Gtk::Allocation alloc = get_allocation();

    int grid_x = (int)((win->b.num_bowls + 2) * event->x / alloc.get_width());
    int grid_y = (event->y / alloc.get_height() <= .5)? 0 : 1;

    if(grid_x > 0 && grid_x < win->b.num_bowls + 1 && !win->b.finished())
    {
        if(win->player == 1 && grid_y == 1)
        {
            win->move(grid_x - 1);
        }

        if(win->player == 2 && grid_y == 0);
    }

    return true;
}

Mancala_win::Mancala_win():
    main_box(Gtk::ORIENTATION_VERTICAL),
    hint_box(Gtk::ORIENTATION_HORIZONTAL),
    new_game_box(Gtk::ORIENTATION_HORIZONTAL),
    hint_b("Hint"),
    new_game_b("New Game"),
    draw(this),
    player(1)
{
    // set window properties
    set_border_width(10);
    set_default_size(800,400);
    set_title("Mancala");

    // add widgets to contatiners
    add(main_box);
    main_box.pack_start(player_label, Gtk::PACK_SHRINK);

    main_box.pack_start(draw);

    main_box.pack_end(new_game_box, Gtk::PACK_SHRINK);
    new_game_box.pack_start(new_game_b, Gtk::PACK_EXPAND_PADDING);

    main_box.pack_end(hint_box, Gtk::PACK_SHRINK);
    hint_box.pack_start(hint_b, Gtk::PACK_EXPAND_PADDING);

    hint_b.signal_clicked().connect(sigc::mem_fun(*this, &Mancala_win::hint));

    new_game_b.signal_clicked().connect(sigc::mem_fun(*this, &Mancala_win::new_game));

    // set all labels for number of seeds
    update_board();

    show_all_children();
}

// make a move (called by button signals)
void Mancala_win::move(const int i)
{
    if(b.bowls[b.p1_start + i].count <= 0)
        return;

    if(!b.move(i))
    {
        //player = (player == 1)? 2 : 1;
        b.swapsides();

        int ai_move = 0;
        do
        {
            if(b.finished())
                break;
            ai_move = choosemove(b);
        }
        while(b.move(ai_move));

        b.swapsides();
    }

    update_board();

    // check to see if the game is over
    if(b.finished())
    {
        Glib::ustring msg;
        // check for a tie
        if(b.bowls[b.p1_store].count == b.bowls[b.p2_store].count)
            msg = "Tie";

        // determine the current player, and then see if they won or lost
        else if(player == 1)
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                msg = "Player 1 wins";
            else
                msg = "Player 2 wins";
        else
            if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
                msg = "Player 2 wins";
            else
                msg = "Player 1 wins";

        // was the win full of win?
        if(abs(b.bowls[b.p1_store].count - b.bowls[b.p2_store].count) >= 10)
            msg += "\nFATALITY";

        //create and show a dialog announcing the winner
        Gtk::MessageDialog dlg(*this, "Game Over");
        dlg.set_secondary_text(msg);
        dlg.run();

        //make the game unplayable
        hint_b.set_state(Gtk::STATE_INSENSITIVE);
    }
}

// get a hint, will highlight a bowl
void Mancala_win::hint()
{
    // use AI function to find best move
    int bestmove = choosemove(b);
}

// start a new game
void Mancala_win::new_game()
{
    hint_b.set_state(Gtk::STATE_NORMAL);
    b = Board();
    update_board();
}

// update the numbers for each bowl / store
void Mancala_win::update_board()
{
    // Show who's turn it is TODO: ugly
    if(player == 1)
        player_label.set_text("Player 1");
    else
        player_label.set_text("Player 2");
    draw.queue_draw();
}
