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

// Random double between 0 and 1 
double randd()
{
    float f = rand();
    return f / RAND_MAX;
}

std::vector<double> rand_pos(const std::vector<double> & ul, double width, double height)
{
    auto pos = ul;
    double theta = randd() * 2 * M_PI;
    double r = randd();
    pos[0] += cos(theta) * r * width * .20;
    pos[1] += sin(theta) * r * height * .20;
    return pos;
}

Mancala_bead::Mancala_bead(const std::vector<double> & Pos, const int Color_i):
    pos(Pos), color_i(Color_i)
{}

Mancala_bead_bowl::Mancala_bead_bowl(const std::vector<double> Ul, const int Num, const double Width,
    const double Height):
    ul(Ul), width(Width), height(Height), next(NULL), across(NULL)
{
    beads = std::vector<Mancala_bead>(Num);

    if(!beads.empty())
        redist_beads();
    for(int i = 0; i < Num; ++i)
        beads[i].color_i = rand() % Mancala_draw::num_colors;
}

void Mancala_bead_bowl::add_bead(const Mancala_bead & new_bead)
{
    beads.push_back(new_bead);

    if(beads.size() <= 4)
        redist_beads();
    else
        beads.back().pos = rand_pos(ul, width, height);
}

void Mancala_bead_bowl::redist_beads()
{
    // center if only one bead
    if(beads.size() == 1)
    {
        beads[0].pos = ul;
        return;
    }

    // for 2-4, use jittered grid dist
    int num_dist_beads = std::min((int)beads.size(), 4);
    std::vector<std::vector<double>> grid_pos(num_dist_beads, std::vector<double>(4));

    switch(num_dist_beads)
    {
    case 2:
        grid_pos[0] = {ul[0], ul[1] - height * .1, height * .1, width};
        grid_pos[1] = {ul[0], ul[1] + height * .1, height * .1, width};
        break;
    case 3:
        grid_pos[0] = {ul[0] + width * .05, ul[1] - height * .1, height * .1, width * .1};
        grid_pos[1] = {ul[0] + width * .1, ul[1] + height * .1, height * .1, width * .1};
        grid_pos[2] = {ul[0] - width * .1, ul[1] + height * .1, height * .1, width * .1};
        break;
    case 4:
    default:
        grid_pos[0] = {ul[0] - width * .1, ul[1] - height * .1, height * .1, width * .1};
        grid_pos[1] = {ul[0] + width * .1, ul[1] - height * .1, height * .1, width * .1};
        grid_pos[2] = {ul[0] + width * .1, ul[1] + height * .1, height * .1, width * .1};
        grid_pos[3] = {ul[0] - width * .1, ul[1] + height * .1, height * .1, width * .1};
        break;
    }

    for(int i = 0; i < num_dist_beads; ++i)
            beads[i].pos = rand_pos({grid_pos[i][0], grid_pos[i][1]}, grid_pos[i][2], grid_pos[i][3]);

    // for >4 use random dist
    if(beads.size() > 4)
        for(size_t i = 4; i < beads.size(); ++i)
            beads[i].pos = rand_pos(ul, width, height);

}

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
        bead_s_img = Gdk::Pixbuf::create_from_file("img/bead_s.png");
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_red.png"));
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_green.png"));
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_blue.png"));
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_yellow.png"));
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_magenta.png"));
        bead_imgs.push_back(Gdk::Pixbuf::create_from_file("img/bead_cyan.png"));
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr<<"File Error: "<<ex.what()<<std::endl;
    }
    catch(const Gdk::PixbufError& ex)
    {
        std::cerr<<"Pixbuf Error: "<< ex.what()<<std::endl;
    }

    set_gui_bowls();
}

void Mancala_draw::set_gui_bowls()
{
    int num_cells = win->b.num_bowls + 2;
    double inv_num_cells = 1.0 / num_cells;

    l_store = Mancala_bead_bowl({0.0, .25}, 0, inv_num_cells, 1.0);
    r_store = Mancala_bead_bowl({1.0 - inv_num_cells, .25}, 0, inv_num_cells, 1.0);

    top_row.clear();
    bottom_row.clear();

    for(int i = 0; i < win->b.num_bowls; ++i)
    {
        top_row.push_back(Mancala_bead_bowl({(double)(i + 1) * inv_num_cells, 0.0}, win->b.num_seeds,
            inv_num_cells, .5));
        bottom_row.push_back(Mancala_bead_bowl({(double)(i + 1) * inv_num_cells, .5}, win->b.num_seeds,
            inv_num_cells, .5));
    }

    // set up next bowl ptrs
    l_store.next = &bottom_row[0];
    r_store.next = &top_row.back();
    for(size_t i = 0; i < bottom_row.size() - 1; ++i)
        bottom_row[i].next = &bottom_row[i + 1];
    for(size_t i = 1; i < top_row.size() ; ++i)
        top_row[i].next = &top_row[i - 1];
    top_row[0].next = &l_store;
    bottom_row.back().next = &r_store;
    for(size_t i = 0; i < top_row.size(); ++i)
    {
        bottom_row[i].across = &top_row[i];
        top_row[i].across = &bottom_row[i];
    }
}

void Mancala_draw::gui_move(const int i, const Mancala_player p)
{
    Mancala_bead_bowl * hand = (p == MANCALA_P1)? &bottom_row[i] : &top_row[top_row.size() - i - 1];
    Mancala_bead_bowl * curr = hand;
    Mancala_bead_bowl * store = (p == MANCALA_P1)? &r_store: &l_store;
    Mancala_bead_bowl * wrong_store = (p == MANCALA_P1)? &l_store: &r_store;
    while(!hand->beads.empty())
    {
        curr = curr->next;
        if(curr == wrong_store)
            curr = curr->next;
        curr->add_bead(hand->beads.back());
        hand->beads.pop_back();
    }

    if(curr->beads.size() == 1 && curr->across != NULL && curr->across->beads.size() > 0)
    {
        auto pos = rand_pos(store->ul, store->width, store->height);
        store->beads.push_back(Mancala_bead(pos, curr->beads[0].color_i));
        curr->beads.clear();
        for(auto & i: curr->across->beads)
        {
            store->add_bead(i);
        }
        curr->across->beads.clear();
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
    for(auto & j: l_store.beads)
    {
        draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
            alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
        draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
            alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
    }

    // draw # for left store
    draw_num(cr, font, win->b.bowls[win->b.p2_store].count, alloc.get_width() * .5 * inv_num_cells, 
        alloc.get_height() * .5);

    // draw bg for right store
    draw_img(cr, bg_store, alloc.get_width() * (1.0 - inv_num_cells), 0,
        alloc.get_width() / (bg_store->get_width() - .5) * inv_num_cells,
        alloc.get_height() / (bg_store->get_height() - .5));

    for(auto & j: r_store.beads)
    {
        draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
            alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
        draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
            alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
    }

    // draw # for right store
    draw_num(cr, font, win->b.bowls[win->b.p1_store].count, alloc.get_width() * (1.0 - .5 * inv_num_cells),
        alloc.get_height() * .5);

    // draw bowls
    for(int i = 0; i < win->b.num_bowls; ++i)
    {
        // draw upper row hint
        if(win->player == MANCALA_P2 && win->show_hint && win->hint_i == i)
        {
            draw_img(cr, hint_img, alloc.get_width() * (i + 1) * inv_num_cells, 0.0,
                alloc.get_width() / (hint_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (hint_img->get_height() - .5) * .5);
        }

        // upper row bgs
        draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, 0,
            alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

        // upper row beads
        for(auto & j: top_row[i].beads)
        {
            draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
            draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
        }

        // upper row txt
        draw_num(cr, font, win->b.bowls[win->b.p1_start + win->b.bowls[i].across].count,
            alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells, alloc.get_height() * .25);

        // draw lower row hint
        if(win->player == MANCALA_P1 && win->show_hint && win->hint_i == i)
        {
            draw_img(cr, hint_img, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
                alloc.get_width() / (hint_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (hint_img->get_height() - .5) * .5);
        }

        // lower row bgs
        draw_img(cr, bg_bowl, alloc.get_width() * (i + 1) * inv_num_cells, .5 * alloc.get_height(),
            alloc.get_width() / (bg_bowl->get_width() - .5) * inv_num_cells,
            alloc.get_height() / (bg_bowl->get_height() - .5) * .5);

        // lower row beads
        for(auto & j: bottom_row[i].beads)
        {
            draw_img(cr, bead_imgs[j.color_i], alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_imgs[j.color_i]->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_imgs[j.color_i]->get_height() - .5) * .5);
            draw_img(cr, bead_s_img, alloc.get_width() * j.pos[0], alloc.get_height() * j.pos[1],
                alloc.get_width() / (bead_s_img->get_width() - .5) * inv_num_cells,
                alloc.get_height() / (bead_s_img->get_height() - .5) * .5);
        }

        // lower row txt
        draw_num(cr, font, win->b.bowls[win->b.p1_start + i].count, alloc.get_width() * (2 * i + 3) * .5 * inv_num_cells,
            alloc.get_height() * .75);
    }

    return true;
}

bool Mancala_draw::mouse_down(GdkEventButton * event)
{
    if(win->moving.test_and_set())
        return true;

    Gtk::Allocation alloc = get_allocation();

    int grid_x = (int)((win->b.num_bowls + 2) * event->x / alloc.get_width());
    int grid_y = (event->y / alloc.get_height() <= .5)? 0 : 1;

    if(grid_x > 0 && grid_x < win->b.num_bowls + 1 && !win->game_over)
        if((!win->p1_ai && win->player == MANCALA_P1 && grid_y == 1) || (!win->p2_ai && win->player == MANCALA_P2 && grid_y == 0))
            win->move(grid_x - 1);

    win->moving.clear();
    return true;
}

Mancala_win::Mancala_win():
    main_box(Gtk::ORIENTATION_VERTICAL),
    draw(this),
    player(MANCALA_P1),
    show_hint(false),
    game_over(false),
    p1_ai(false),
    p2_ai(true)
{
    moving.test_and_set();

    // set window properties
    set_default_size(800,400);
    set_title("Mancala");

    // build menu and toolbar
    actgrp = Gtk::ActionGroup::create();
    actgrp->add(Gtk::Action::create("Game", "Game"));
    actgrp->add(Gtk::Action::create("Game_newgame", Gtk::Stock::NEW, "_New Game", "Give up and start a new game"),
        sigc::mem_fun(*this, &Mancala_win::new_game));
    actgrp->add(Gtk::Action::create("Game_hint", Gtk::Stock::HELP, "Hint", "Get a hint. May be misleading"),
        sigc::mem_fun(*this, &Mancala_win::hint));
    actgrp->add(Gtk::Action::create("Game_quit", Gtk::Stock::QUIT, "_Quit", "Quit"),
        sigc::mem_fun(*this, &Mancala_win::hide));
    actgrp->add(Gtk::Action::create("Players", "Players"));

    uiman = Gtk::UIManager::create();
    uiman->insert_action_group(actgrp);
    add_accel_group(uiman->get_accel_group());

    Glib::ustring ui_str =
    "<ui>"
    "   <menubar name='MenuBar'>"
    "       <menu action='Game'>"
    "           <menuitem action='Game_newgame'/>"
    "           <menuitem action='Game_hint'/>"
    "           <separator/>"
    "           <menuitem action='Game_quit'/>"
    "       </menu>"
    "       <menu action='Players'>"
    "       </menu>"
    "   </menubar>"
    "   <toolbar name='ToolBar'>"
    "       <toolitem action='Game_newgame'/>"
    "       <toolitem action='Game_hint'/>"
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

    // set timer to make AI moves when able. Check every 50ms
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Mancala_win::ai_timer), 50);

    // set all labels for number of seeds
    update_board();

    show_all_children();
    moving.clear();
}

// make a move (called by button signals)
void Mancala_win::move(const int i)
{
    bool extra_move;
    if(player == MANCALA_P1)
    {
        if(b.bowls[b.p1_start + i].count <= 0)
            return;
        extra_move = b.move(i);
        draw.gui_move(i, MANCALA_P1);
        if(!extra_move)
            player = MANCALA_P2;
    }
    else
    {
        if(b.bowls[b.bowls[b.p1_start + i].across].count <= 0)
            return;
        b.swapsides();
        extra_move = b.move(b.num_bowls - i - 1);
        draw.gui_move(b.num_bowls - i - 1, MANCALA_P2);
        if(!extra_move)
            player = MANCALA_P1;
        b.swapsides();
    }

    // check to see if the game is over
    if(b.finished())
        disp_winner();
    show_hint = false;

    update_board();
}

bool Mancala_win::ai_timer()
{
    if(moving.test_and_set())
        return true;
    if(!game_over)
    {
        if((player == MANCALA_P1 && p1_ai) || (player == MANCALA_P2 && p2_ai))
            ai_move();
    }
    moving.clear();
    return true;
}

// Have the AI make a move
void Mancala_win::ai_move()
{
    if(player == MANCALA_P2)
        b.swapsides();
    bool ai_extra_move = false;
    do
    {
        if(b.finished())
            break;
        int ai_move = choosemove(b);
        ai_extra_move = b.move(ai_move);
        if(player == MANCALA_P1)
            draw.gui_move(ai_move, MANCALA_P1);
        else
            draw.gui_move(ai_move, MANCALA_P2);
    }
    while(ai_extra_move);

    if(player == MANCALA_P2)
    {
        player = MANCALA_P1;
        b.swapsides();
    }
    else
        player = MANCALA_P2;

    // check to see if the game is over
    if(b.finished())
        disp_winner();
    show_hint = false;

    update_board();
}

// display the winner, end the game
void Mancala_win::disp_winner()
{
    // deactivate hint feature
    game_over = true;
    actgrp->get_action("Game_hint")->set_sensitive(false);

    // create and show a dialog announcing the winner
    Glib::ustring msg;

    // check for a tie
    if(b.bowls[b.p1_store].count == b.bowls[b.p2_store].count)
        msg = "Tie";
    else
        if(b.bowls[b.p1_store].count > b.bowls[b.p2_store].count)
            msg = "Player 1 wins";
        else
            msg = "Player 2 wins";

    // was the win full of win?
    if(abs(b.bowls[b.p1_store].count - b.bowls[b.p2_store].count) >= 10)
        msg += "\nFATALITY";

    Gtk::MessageDialog dlg(*this, "Game Over");
    dlg.set_secondary_text(msg);
    dlg.run();
}

// get a hint, will highlight a bowl
void Mancala_win::hint()
{
    if(moving.test_and_set())
        return;
    if(game_over)
        return;
    // use AI function to find best move
    if(player == MANCALA_P2)
        b.swapsides();

    hint_i = choosemove(b);

    if(player == MANCALA_P2)
    {
        b.swapsides();
        hint_i = b.num_bowls - hint_i - 1;
    }

    show_hint = true;
    update_board();
    moving.clear();
}

// start a new game
void Mancala_win::new_game()
{
    if(moving.test_and_set())
        return;
    // Reactivate hint feature
    game_over = false;
    actgrp->get_action("Game_hint")->set_sensitive(true);

    player = MANCALA_P1;
    b = Board();
    draw.set_gui_bowls();
    show_hint = false;
    update_board();
    moving.clear();
}

// update the numbers for each bowl / store
void Mancala_win::update_board()
{
    // Show who's turn it is TODO: ugly
    if(player == MANCALA_P1)
        player_label.set_text("Player 1");
    else
        player_label.set_text("Player 2");

    draw.queue_draw();
}
