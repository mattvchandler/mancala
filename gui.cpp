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
        bg_bowl = Gdk::Pixbuf::create_from_file("img/bg_bowl.png");
        bg_board = Gdk::Pixbuf::create_from_file("img/bg_board.png");
        hint_img = Gdk::Pixbuf::create_from_file("img/hint.png");
        beads.push_back(Gdk::Pixbuf::create_from_file("img/bead_1.png"));
        beads.push_back(Gdk::Pixbuf::create_from_file("img/bead_2.png"));
        beads.push_back(Gdk::Pixbuf::create_from_file("img/bead_3.png"));
        beads.push_back(Gdk::Pixbuf::create_from_file("img/bead_4.png"));
        beads.push_back(Gdk::Pixbuf::create_from_file("img/bead_5.png"));
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

void Mancala_draw::draw_img(const Cairo::RefPtr<Cairo::Context>& cr, const Glib::RefPtr<Gdk::Pixbuf>&  img,
    const double translate_x, const double translate_y, const double scale_x, const double scale_y)
{
    cr->save();
    cr->translate(translate_x, translate_y);
    cr->scale(scale_x, scale_y);
    Gdk::Cairo::set_source_pixbuf(cr, img);
    cr->paint();
    cr->restore();
}

void Mancala_draw::draw_num(const Cairo::RefPtr<Cairo::Context>& cr, const Pango::FontDescription & font,
    const int num, const double x, const double y)
{
    std::ostringstream str;
    int txt_w, txt_h;
    str<<num;
    Glib::RefPtr<Pango::Layout> txt = create_pango_layout(str.str());
    txt->set_font_description(font);
    txt->get_pixel_size(txt_w, txt_h);
    cr->move_to(x - .5 * txt_w, y - .5 * txt_h);
    txt->show_in_cairo_context(cr);
}

bool Mancala_draw::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int num_cells = win->b.num_bowls + 2;
    double inv_num_cells = 1.0 / num_cells;

    Gtk::Allocation alloc = get_allocation();

    Pango::FontDescription font("Monospace");
    font.set_size(std::min(alloc.get_width(), alloc.get_height()) * .1 * Pango::SCALE);
    cr->set_source_rgb(0.0, 0.0, 0.0);

    // draw bg
    draw_img(cr, bg_board, 0.0, 0.0, alloc.get_width() / (bg_board->get_width() - .5),
        alloc.get_height() / (bg_board->get_height() - .5));

    // draw bg for left store
    draw_img(cr, bg_store, 0.0, 0.0, alloc.get_width() / (bg_store->get_width() - .5) * inv_num_cells,
        alloc.get_height() / (bg_store->get_height() - .5));

    // l store beads
    int num_beads = win->b.bowls[win->b.p2_store].count;
    int num_beads_shown = (num_beads > 5)? 5 : num_beads;
    if(num_beads_shown > 0)
    {
        draw_img(cr, beads[num_beads_shown - 1], 0.0, alloc.get_height() * .25,
            alloc.get_width() / (beads[num_beads_shown - 1]->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (beads[num_beads_shown - 1]->get_height() - .5) * .5);
    }

    // draw # for left store
    draw_num(cr, font, win->b.bowls[win->b.p2_store].count, alloc.get_width() * .5 * inv_num_cells, 
        alloc.get_height() * .5);

    // draw bg for right store
    draw_img(cr, bg_store, alloc.get_width() * (1.0 - inv_num_cells), 0,
        alloc.get_width() / (bg_store->get_width() - .5) * inv_num_cells,
        alloc.get_height() / (bg_store->get_height() - .5));

    num_beads = win->b.bowls[win->b.p1_store].count;
    num_beads_shown = (num_beads > 5)? 5 : num_beads;
    if(num_beads_shown > 0)
    {
        draw_img(cr, beads[num_beads_shown - 1], alloc.get_width() * (1.0 - inv_num_cells), alloc.get_height() * .25,
            alloc.get_width() / (beads[num_beads_shown - 1]->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (beads[num_beads_shown - 1]->get_height() - .5) * .5);
    }

    // draw # for right store
    draw_num(cr, font, win->b.bowls[win->b.p1_store].count, alloc.get_width() * (1.0 - .5 * inv_num_cells),
        alloc.get_height() * .5);

    // draw bowls
    for(int i = 0; i < win->b.num_bowls; ++i)
    {
        //upper row bgs
        draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, 0,
            alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

        //upper row beads
        int num_beads = win->b.bowls[win->b.bowls[win->b.p1_start + i].across].count;
        int num_beads_shown = (num_beads > 5)? 5 : num_beads;
        if(num_beads_shown > 0)
        {
            draw_img(cr, beads[num_beads_shown - 1], alloc.get_width() * (i + 1) * inv_num_cells, 0,
                alloc.get_width() / (beads[num_beads_shown - 1]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (beads[num_beads_shown - 1]->get_height() - .5) * .5);
        }

        //upper row txt
        draw_num(cr, font, num_beads, alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells,
            alloc.get_height() * .25);

        // draw hint
        if(win->show_hint && win->hint_i == i)
        {
            draw_img(cr, hint_img, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
                alloc.get_width() / (hint_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (hint_img->get_height() - .5) * .5);
        }

        // lower row bgs
        draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
            alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

        //lower row beads
        num_beads = win->b.bowls[win->b.p1_start + i].count;
        num_beads_shown = (num_beads > 5)? 5 : num_beads;
        if(num_beads_shown > 0)
        {
            draw_img(cr, beads[num_beads_shown - 1],
                alloc.get_width() * (i + 1) * inv_num_cells,
                .5 * alloc.get_height(),
                alloc.get_width() / (beads[num_beads_shown - 1]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (beads[num_beads_shown - 1]->get_height() - .5) * .5);
        }

        //lower row txt
        draw_num(cr, font, num_beads, alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells,
            alloc.get_height() * .75);
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
    player(1),
    show_hint(false)
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

    show_hint = false;
}

// get a hint, will highlight a bowl
void Mancala_win::hint()
{
    // use AI function to find best move
    hint_i = choosemove(b);
    show_hint = true;
    update_board();
}

// start a new game
void Mancala_win::new_game()
{
    hint_b.set_state(Gtk::STATE_NORMAL);
    b = Board();
    show_hint = false;
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
